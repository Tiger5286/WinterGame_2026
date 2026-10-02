using System;

namespace LocalCompletion
{
    // VSに依存しない処理。補完候補として表示した文字だけを挿入する。
    internal static class CompletionCore
    {
        public static Uri LocalEndpoint(string endpoint)
        {
            if (!Uri.TryCreate(endpoint, UriKind.Absolute, out var uri) ||
                uri.Scheme != "http" || !uri.IsLoopback ||
                !string.IsNullOrEmpty(uri.UserInfo) || uri.AbsolutePath != "/" ||
                !string.IsNullOrEmpty(uri.Query) || !string.IsNullOrEmpty(uri.Fragment))
                throw new ArgumentException("接続先は http://localhost:11434 のような、このPC内のHTTPアドレスを指定してください。");
            return new Uri(uri, "api/generate");
        }

        public static string BuildPrompt(string prefix, string suffix)
        {
            // Qwen CoderのFIM形式。カーソル前後のコードから間に入る部分だけを生成する。
            return "<|fim_prefix|>" + prefix + "<|fim_suffix|>" + suffix + "<|fim_middle|>";
        }

        public static string FirstLine(string response)
        {
            if (string.IsNullOrEmpty(response)) return "";
            int end = response.IndexOfAny(new[] { '\r', '\n' });
            string line = end < 0 ? response : response.Substring(0, end);
            foreach (string marker in new[] { "<|", "```" })
            {
                int pos = line.IndexOf(marker, StringComparison.Ordinal);
                if (pos >= 0) line = line.Substring(0, pos);
            }
            // インデントは残し、説明や制御文字がエディターに混ざるのを防ぐ。
            foreach (char ch in line)
                if (char.IsControl(ch) && ch != '\t') return "";
            return string.IsNullOrWhiteSpace(line) ? "" : line.TrimEnd();
        }

        public static bool IsCurrent(int requestVersion, int currentVersion, int requestedPosition, int caretPosition)
            => requestVersion == currentVersion && requestedPosition == caretPosition;
    }
}
