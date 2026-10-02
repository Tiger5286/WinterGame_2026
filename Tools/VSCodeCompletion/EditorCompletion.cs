using System;
using System.ComponentModel.Composition;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Editor;
using Microsoft.VisualStudio.Language.Intellisense;
using Microsoft.VisualStudio.Language.Intellisense.AsyncCompletion;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Editor;
using Microsoft.VisualStudio.Text.Operations;
using Microsoft.VisualStudio.TextManager.Interop;
using Microsoft.VisualStudio.Utilities;

namespace LocalCompletion
{
    [Export(typeof(IVsTextViewCreationListener))]
    [ContentType("C/C++")]
    [TextViewRole(PredefinedTextViewRoles.Editable)]
    internal sealed class CompletionViewListener : IVsTextViewCreationListener
    {
        [Import] internal IVsEditorAdaptersFactoryService Adapters = null;
        [Import] internal ICompletionBroker CompletionBroker = null;
        [Import] internal IAsyncCompletionBroker AsyncCompletionBroker = null;
        [Import] internal ITextUndoHistoryRegistry UndoRegistry = null;

        [Export(typeof(AdornmentLayerDefinition))]
        [Name(CompletionController.LayerName)]
        [Order(After = PredefinedAdornmentLayers.Text, Before = PredefinedAdornmentLayers.Caret)]
        internal AdornmentLayerDefinition Layer = null;

        public void VsTextViewCreated(IVsTextView textViewAdapter)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            var view = Adapters.GetWpfTextView(textViewAdapter);
            if (view == null || view.Properties.ContainsProperty(typeof(CompletionController))) return;
            // プロジェクションビューは挿入位置が異なることがあるため対象外にする。
            if (view.TextBuffer != view.TextDataModel.DocumentBuffer) return;
            var controller = new CompletionController(view, textViewAdapter, CompletionBroker, AsyncCompletionBroker, UndoRegistry);
            view.Properties.AddProperty(typeof(CompletionController), controller);
        }
    }

    internal sealed class CompletionController : IOleCommandTarget
    {
        public const string LayerName = "LocalCompletion.GhostText";
        private readonly IWpfTextView view;
        private readonly IVsTextView adapter;
        private readonly ICompletionBroker completionBroker;
        private readonly IAsyncCompletionBroker asyncCompletionBroker;
        private readonly ITextUndoHistoryRegistry undoRegistry;
        private readonly IAdornmentLayer layer;
        private readonly DispatcherTimer timer;
        private IOleCommandTarget next;
        private CancellationTokenSource pending;
        private ITextSnapshot suggestionSnapshot;
        private int suggestionPosition;
        private string suggestion;
        private bool ghostVisible;
        private bool closed;
        private DateTime retryAfter;

        public CompletionController(IWpfTextView view, IVsTextView adapter, ICompletionBroker completionBroker,
            IAsyncCompletionBroker asyncCompletionBroker, ITextUndoHistoryRegistry undoRegistry)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            this.view = view; this.adapter = adapter; this.completionBroker = completionBroker;
            this.asyncCompletionBroker = asyncCompletionBroker; this.undoRegistry = undoRegistry;
            layer = view.GetAdornmentLayer(LayerName);
            timer = new DispatcherTimer(DispatcherPriority.Background, view.VisualElement.Dispatcher);
            timer.Tick += OnTimer;
            ErrorHandler.ThrowOnFailure(adapter.AddCommandFilter(this, out next));
            view.TextBuffer.Changed += OnTextChanged;
            view.Caret.PositionChanged += OnCaretChanged;
            view.Selection.SelectionChanged += OnSelectionChanged;
            view.LostAggregateFocus += OnLostFocus;
            view.LayoutChanged += OnLayoutChanged;
            view.Closed += OnClosed;
            SettingsStore.Changed += OnSettingsChanged;
        }

        private bool HasIntelliSense() => completionBroker.IsCompletionActive(view) || asyncCompletionBroker.IsCompletionActive(view);

        private bool CanComplete()
        {
            if (closed || !view.HasAggregateFocus || !view.Selection.IsEmpty || view.Caret.InVirtualSpace || HasIntelliSense()) return false;
            var caret = view.Caret.Position.BufferPosition;
            // 初版は行末での1行補完。既存の行内コードを覆わずに候補を表示する。
            return caret.Snapshot == view.TextSnapshot && caret.Position == caret.GetContainingLine().End.Position &&
                !view.TextBuffer.IsReadOnly(caret.Position);
        }

        private void Invalidate()
        {
            timer.Stop();
            pending?.Cancel();
            pending = null;
            suggestion = null;
            suggestionSnapshot = null;
            ghostVisible = false;
            layer.RemoveAllAdornments();
        }

        private void Queue()
        {
            if (!SettingsStore.Current.Enabled || DateTime.UtcNow < retryAfter) return;
            timer.Interval = TimeSpan.FromMilliseconds(SettingsStore.Current.DelayMilliseconds);
            timer.Start();
        }

        private void OnTextChanged(object sender, TextContentChangedEventArgs e) { Invalidate(); Queue(); }
        private void OnCaretChanged(object sender, CaretPositionChangedEventArgs e)
        {
            bool wasWaiting = timer.IsEnabled;
            Invalidate();
            if (wasWaiting) Queue();
        }
        private void OnSelectionChanged(object sender, EventArgs e) { if (!view.Selection.IsEmpty) Invalidate(); }
        private void OnLostFocus(object sender, EventArgs e) => Invalidate();
        private void OnSettingsChanged() => Invalidate();
        // DispatcherTimerのイベント境界ですべての例外を処理する。
#pragma warning disable VSTHRD100
        private async void OnTimer(object sender, EventArgs e)
        {
            timer.Stop();
            try { await RequestAsync(false); }
            catch (Exception ex)
            {
                await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
                CompletionPackage.Status("Local Completion: " + ex.Message);
            }
        }
#pragma warning restore VSTHRD100

        public async Task RequestAsync(bool manual)
        {
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
            if (closed) return;
            Invalidate();
            if (!CanComplete())
            {
                if (manual) CompletionPackage.Status("C++ファイルの行末にカーソルを置き、IntelliSense候補を閉じてから補完してください。");
                return;
            }
            var settings = SettingsStore.Current;
            var snapshot = view.TextSnapshot;
            int position = view.Caret.Position.BufferPosition.Position;
            int start = Math.Max(0, position - settings.PrefixCharacters);
            string prefix = snapshot.GetText(start, position - start);
            string suffix = snapshot.GetText(position, Math.Min(settings.SuffixCharacters, snapshot.Length - position));
            var request = new CancellationTokenSource();
            pending = request;
            try
            {
                if (manual) CompletionPackage.Status("ローカルAIで補完を生成中… 初回はモデルの読み込みに時間がかかります。");
                string result = await OllamaClient.GenerateAsync(settings, prefix, suffix, request.Token);
                // 通信中の入力・移動・ファイル切替があれば古い候補を破棄する。
                if (request.IsCancellationRequested || closed || pending != request || snapshot != view.TextSnapshot ||
                    !CompletionCore.IsCurrent(snapshot.Version.VersionNumber, view.TextSnapshot.Version.VersionNumber,
                        position, view.Caret.Position.BufferPosition.Position) || !CanComplete()) return;
                suggestion = result;
                suggestionSnapshot = snapshot;
                suggestionPosition = position;
                Draw();
                if (manual) CompletionPackage.Status(string.IsNullOrEmpty(result) ? "補完候補はありませんでした。" : "Tabで採用 / Escで取消");
                retryAfter = DateTime.MinValue;
            }
            catch (OperationCanceledException)
            {
                if (!request.IsCancellationRequested && !closed)
                {
                    retryAfter = DateTime.UtcNow.AddSeconds(15);
                    CompletionPackage.Status("Local Completion: タイムアウトしました。接続テストを試してください。");
                }
            }
            catch (Exception ex)
            {
                if (!request.IsCancellationRequested && !closed)
                {
                    retryAfter = DateTime.UtcNow.AddSeconds(30);
                    CompletionPackage.Status(ex is System.Net.Http.HttpRequestException
                        ? "Local Completion: Ollamaに接続できません。Setup.ps1を実行するか、Ollamaを起動してください。"
                        : "Local Completion: " + ex.Message);
                }
            }
            finally
            {
                if (pending == request) pending = null;
                request.Dispose();
            }
        }

        private void OnLayoutChanged(object sender, TextViewLayoutChangedEventArgs e) => Draw();

        private void Draw()
        {
            ghostVisible = false;
            layer.RemoveAllAdornments();
            if (closed || string.IsNullOrEmpty(suggestion) || suggestionSnapshot != view.TextSnapshot || !CanComplete()) return;
            var point = new SnapshotPoint(suggestionSnapshot, suggestionPosition);
            var visible = view.TextViewLines.FormattedSpan;
            // EOFのカーソルも対象。SnapshotSpan.Containsでは終端が除外される。
            if (point.Position < visible.Start.Position || point.Position > visible.End.Position) return;
            var line = view.TextViewLines.GetTextViewLineContainingBufferPosition(point);
            var bounds = line.GetCharacterBounds(point);
            var format = view.FormattedLineSource.DefaultTextProperties;
            var brush = format.ForegroundBrush.Clone();
            brush.Opacity = 0.5;
            var ghost = new TextBlock {
                Text = suggestion.Replace("\t", new string(' ', view.Options.GetOptionValue(DefaultOptions.TabSizeOptionId))),
                Foreground = brush, FontFamily = format.Typeface.FontFamily, FontSize = format.FontRenderingEmSize,
                FontStyle = format.Typeface.Style, FontWeight = format.Typeface.Weight, IsHitTestVisible = false
            };
            Canvas.SetLeft(ghost, bounds.Left);
            Canvas.SetTop(ghost, line.TextTop);
            ghostVisible = layer.AddAdornment(AdornmentPositioningBehavior.TextRelative, new SnapshotSpan(point, 0), this, ghost,
                (tag, element) => ghostVisible = false);
        }

        private bool Accept()
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            if (!ghostVisible || string.IsNullOrEmpty(suggestion) || suggestionSnapshot != view.TextSnapshot || !CanComplete() ||
                view.Caret.Position.BufferPosition.Position != suggestionPosition) return false;
            string text = suggestion;
            int position = suggestionPosition;
            Invalidate();
            undoRegistry.TryGetHistory(view.TextBuffer, out var history);
            using (var transaction = history?.CreateTransaction("ローカルAIの補完を採用"))
            {
                var after = view.TextBuffer.Insert(position, text);
                view.Caret.MoveTo(new SnapshotPoint(after, position + text.Length));
                transaction?.Complete();
            }
            return true;
        }

        public int QueryStatus(ref Guid group, uint count, OLECMD[] commands, IntPtr text)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            return next.QueryStatus(ref group, count, commands, text);
        }

        public int Exec(ref Guid group, uint id, uint options, IntPtr input, IntPtr output)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            if (group == VSConstants.VSStd2K)
            {
                // IntelliSenseが開いている間は既存のTab操作を優先する。
                if (id == (uint)VSConstants.VSStd2KCmdID.TAB && Accept()) return VSConstants.S_OK;
                if (id == (uint)VSConstants.VSStd2KCmdID.CANCEL && (pending != null || timer.IsEnabled || !string.IsNullOrEmpty(suggestion)))
                {
                    bool passThrough = HasIntelliSense();
                    Invalidate();
                    if (!passThrough) return VSConstants.S_OK;
                }
            }
            return next.Exec(ref group, id, options, input, output);
        }

        private void OnClosed(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            closed = true;
            Invalidate();
            timer.Tick -= OnTimer;
            view.TextBuffer.Changed -= OnTextChanged;
            view.Caret.PositionChanged -= OnCaretChanged;
            view.Selection.SelectionChanged -= OnSelectionChanged;
            view.LostAggregateFocus -= OnLostFocus;
            view.LayoutChanged -= OnLayoutChanged;
            view.Closed -= OnClosed;
            SettingsStore.Changed -= OnSettingsChanged;
            adapter.RemoveCommandFilter(this);
        }
    }
}
