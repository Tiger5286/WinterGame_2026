using System;
using System.ComponentModel.Design;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.ComponentModelHost;
using Microsoft.VisualStudio.Editor;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.TextManager.Interop;
using Task = System.Threading.Tasks.Task;

namespace LocalCompletion
{
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
    [Guid(PackageGuid)]
    [ProvideMenuResource("Commands.CTMENU", 1)]
    [ProvideOptionPage(typeof(OptionsPage), "Local Completion", "全般", 0, 0, true)]
    public sealed class CompletionPackage : AsyncPackage
    {
        public const string PackageGuid = "F45C20EF-A86C-42DE-93C7-0F2E1CB05301";
        private static readonly Guid Commands = new Guid("1D2833F7-B5EC-4435-8DB7-45298FA79C16");
        private bool testing;

        protected override async Task InitializeAsync(CancellationToken cancellationToken, IProgress<ServiceProgressData> progress)
        {
            var service = await GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
            await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
            service?.AddCommand(new MenuCommand((s, e) => { _ = JoinableTaskFactory.RunAsync(CompleteAsync); }, new CommandID(Commands, 0x0100)));
            service?.AddCommand(new MenuCommand((s, e) => Toggle(), new CommandID(Commands, 0x0101)));
            service?.AddCommand(new MenuCommand((s, e) => { _ = JoinableTaskFactory.RunAsync(TestAsync); }, new CommandID(Commands, 0x0102)));
        }

        internal static void Status(string message)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            (GetGlobalService(typeof(SVsStatusbar)) as IVsStatusbar)?.SetText(message);
        }

        private async Task CompleteAsync()
        {
            await JoinableTaskFactory.SwitchToMainThreadAsync();
            try
            {
                var manager = await GetServiceAsync(typeof(SVsTextManager)) as IVsTextManager;
                var model = await GetServiceAsync(typeof(SComponentModel)) as IComponentModel;
                if (manager != null && ErrorHandler.Succeeded(manager.GetActiveView(1, null, out var adapter)))
                {
                    var view = model?.GetService<IVsEditorAdaptersFactoryService>().GetWpfTextView(adapter);
                    if (view != null && view.Properties.TryGetProperty(typeof(CompletionController), out CompletionController controller))
                    {
                        await controller.RequestAsync(true);
                        return;
                    }
                }
                Status("C++のソース／ヘッダーファイルを開いてから補完してください。");
            }
            catch (Exception ex) { Status("Local Completion: " + ex.Message); }
        }

        private void Toggle()
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            try
            {
                var s = SettingsStore.Current;
                SettingsStore.Save(new CompletionSettings {
                    Enabled = !s.Enabled, Endpoint = s.Endpoint, Model = s.Model,
                    DelayMilliseconds = s.DelayMilliseconds, TimeoutSeconds = s.TimeoutSeconds,
                    PrefixCharacters = s.PrefixCharacters, SuffixCharacters = s.SuffixCharacters
                });
                Status("Local Completion: 自動補完 " + (SettingsStore.Current.Enabled ? "オン" : "オフ"));
            }
            catch (Exception ex) { Status("Local Completion: " + ex.Message); }
        }

        private async Task TestAsync()
        {
            await JoinableTaskFactory.SwitchToMainThreadAsync();
            if (testing) return;
            testing = true;
            try
            {
                Status("Ollama接続テスト中…");
                string text = await OllamaClient.GenerateAsync(SettingsStore.Current,
                    "int Add(int a, int b)\n{\n    return", "\n}\n", CancellationToken.None);
                VsShellUtilities.ShowMessageBox(this, "接続・生成に成功しました。\n生成例: return" + text,
                    "Local Completion", OLEMSGICON.OLEMSGICON_INFO, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
                Status("Local Completion: 接続テスト完了");
            }
            catch (Exception ex)
            {
                string message = ex is OperationCanceledException ? "タイムアウトしました。Ollamaの起動とモデルを確認してください。" : ex.Message;
                VsShellUtilities.ShowMessageBox(this, message + "\n\nSetup.ps1でセットアップできます。設定: ツール → オプション → Local Completion",
                    "Local Completion", OLEMSGICON.OLEMSGICON_WARNING, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
            }
            finally { testing = false; }
        }
    }
}
