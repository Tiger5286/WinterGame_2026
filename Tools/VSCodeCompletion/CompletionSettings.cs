using System;

namespace LocalCompletion
{
    public sealed class CompletionSettings
    {
        public bool Enabled { get; set; } = true;
        public string Endpoint { get; set; } = "http://localhost:11434";
        public string Model { get; set; } = "qwen2.5-coder:1.5b-base";
        public int DelayMilliseconds { get; set; } = 800;
        public int TimeoutSeconds { get; set; } = 45;
        public int PrefixCharacters { get; set; } = 6000;
        public int SuffixCharacters { get; set; } = 2000;

        public void Validate()
        {
            CompletionCore.LocalEndpoint(Endpoint);
            if (string.IsNullOrWhiteSpace(Model) || Model.IndexOf("cloud", StringComparison.OrdinalIgnoreCase) >= 0)
                throw new ArgumentException("ダウンロード済みのローカルモデル名を指定してください。cloudモデルは使えません。");
            if (DelayMilliseconds < 300 || DelayMilliseconds > 10000)
                throw new ArgumentException("入力待ち時間は300～10000ミリ秒にしてください。");
            if (TimeoutSeconds < 5 || TimeoutSeconds > 120)
                throw new ArgumentException("タイムアウトは5～120秒にしてください。");
            if (PrefixCharacters < 100 || PrefixCharacters > 6000 || SuffixCharacters < 0 || SuffixCharacters > 2000)
                throw new ArgumentException("前方の文字数は100～6000、後方は0～2000にしてください。");
        }
    }

}
