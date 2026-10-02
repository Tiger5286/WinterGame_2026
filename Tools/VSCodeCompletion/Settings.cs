using System;
using System.ComponentModel;
using System.IO;
using System.Xml.Serialization;
using Microsoft.VisualStudio.Shell;

namespace LocalCompletion
{
    internal static class SettingsStore
    {
        private static readonly string FilePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "LocalCompletion", "settings.xml");
        private static CompletionSettings current;
        public static event Action Changed;
        public static CompletionSettings Current => current ?? (current = Load());

        private static CompletionSettings Load()
        {
            try
            {
                if (File.Exists(FilePath))
                    using (var stream = File.OpenRead(FilePath))
                    {
                        var result = (CompletionSettings)new XmlSerializer(typeof(CompletionSettings)).Deserialize(stream);
                        result.Validate();
                        return result;
                    }
            }
            catch (Exception ex) when (ex is IOException || ex is InvalidOperationException || ex is ArgumentException || ex is UnauthorizedAccessException)
            {
                // 設定破損時にもVSのエディターを起動できるよう初期値に戻す。
            }
            return new CompletionSettings();
        }

        public static void Save(CompletionSettings settings)
        {
            settings.Validate();
            Directory.CreateDirectory(Path.GetDirectoryName(FilePath));
            using (var stream = File.Create(FilePath))
                new XmlSerializer(typeof(CompletionSettings)).Serialize(stream, settings);
            current = settings;
            Changed?.Invoke();
        }
    }

    public sealed class OptionsPage : DialogPage
    {
        [Category("補完"), DisplayName("自動補完"), Description("入力後に自動で候補を生成します。オフでも手動補完は使えます。")]
        public bool Enabled { get; set; }
        [Category("接続"), DisplayName("Ollama接続先")]
        public string Endpoint { get; set; }
        [Category("接続"), DisplayName("モデル"), Description("Qwen2.5 Coderのbaseモデルを指定します。例: qwen2.5-coder:1.5b-base")]
        public string Model { get; set; }
        [Category("補完"), DisplayName("入力待ち時間（ミリ秒）")]
        public int DelayMilliseconds { get; set; }
        [Category("接続"), DisplayName("タイムアウト（秒）")]
        public int TimeoutSeconds { get; set; }
        [Category("文脈"), DisplayName("カーソル前の最大文字数")]
        public int PrefixCharacters { get; set; }
        [Category("文脈"), DisplayName("カーソル後の最大文字数")]
        public int SuffixCharacters { get; set; }

        public override void LoadSettingsFromStorage()
        {
            var s = SettingsStore.Current;
            Enabled = s.Enabled; Endpoint = s.Endpoint; Model = s.Model;
            DelayMilliseconds = s.DelayMilliseconds; TimeoutSeconds = s.TimeoutSeconds;
            PrefixCharacters = s.PrefixCharacters; SuffixCharacters = s.SuffixCharacters;
        }

        public override void SaveSettingsToStorage()
        {
            SettingsStore.Save(new CompletionSettings { Enabled = Enabled, Endpoint = Endpoint, Model = Model,
                DelayMilliseconds = DelayMilliseconds, TimeoutSeconds = TimeoutSeconds,
                PrefixCharacters = PrefixCharacters, SuffixCharacters = SuffixCharacters });
        }

        protected override void OnApply(PageApplyEventArgs e)
        {
            try { SaveSettingsToStorage(); base.OnApply(e); }
            catch (Exception ex)
            {
                e.ApplyBehavior = ApplyKind.CancelNoNavigate;
                System.Windows.MessageBox.Show(ex.Message, "Local Completion");
            }
        }
    }
}
