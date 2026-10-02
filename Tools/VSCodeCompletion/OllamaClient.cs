using System;
using System.IO;
using System.Net.Http;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LocalCompletion
{
    internal sealed class OllamaClient
    {
        // ローカルのコードがプロキシやリダイレクト先に送られないようにする。
        private static readonly HttpClient Http = new HttpClient(new HttpClientHandler { UseProxy = false, AllowAutoRedirect = false })
        { Timeout = Timeout.InfiniteTimeSpan };
        private static readonly SemaphoreSlim Gate = new SemaphoreSlim(1, 1);

        public static async Task<string> GenerateAsync(CompletionSettings settings, string prefix, string suffix, CancellationToken token)
        {
            settings.Validate();
            using (var timeout = CancellationTokenSource.CreateLinkedTokenSource(token))
            {
                timeout.CancelAfter(TimeSpan.FromSeconds(settings.TimeoutSeconds));
                await Gate.WaitAsync(timeout.Token).ConfigureAwait(false);
                try
                {
                    var payload = new GenerateRequest { model = settings.Model, prompt = CompletionCore.BuildPrompt(prefix, suffix) };
                    string json;
                    using (var stream = new MemoryStream())
                    {
                        new DataContractJsonSerializer(typeof(GenerateRequest)).WriteObject(stream, payload);
                        json = Encoding.UTF8.GetString(stream.ToArray());
                    }
                    using (var content = new StringContent(json, Encoding.UTF8, "application/json"))
                    using (var response = await Http.PostAsync(CompletionCore.LocalEndpoint(settings.Endpoint), content, timeout.Token).ConfigureAwait(false))
                    {
                        if (!response.IsSuccessStatusCode)
                            throw new InvalidOperationException(response.StatusCode == System.Net.HttpStatusCode.NotFound
                                ? "モデルが見つかりません。Setup.ps1、または ollama pull " + settings.Model + " を実行してください。"
                                : "Ollamaがエラーを返しました: HTTP " + (int)response.StatusCode);
                        string body = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        using (var stream = new MemoryStream(Encoding.UTF8.GetBytes(body)))
                        {
                            var result = (GenerateResponse)new DataContractJsonSerializer(typeof(GenerateResponse)).ReadObject(stream);
                            if (!string.IsNullOrEmpty(result.error)) throw new InvalidOperationException("Ollamaの生成に失敗しました。");
                            return CompletionCore.FirstLine(result.response);
                        }
                    }
                }
                finally { Gate.Release(); }
            }
        }

        [DataContract]
        private sealed class GenerateRequest
        {
            [DataMember] public string model;
            [DataMember] public string prompt;
            [DataMember] public bool stream = false;
            [DataMember] public bool raw = true;
            [DataMember] public string keep_alive = "2m";
            [DataMember] public GenerateOptions options = new GenerateOptions();
        }
        [DataContract]
        private sealed class GenerateOptions
        {
            [DataMember] public int num_predict = 64;
            [DataMember] public int num_ctx = 4096;
            [DataMember] public double temperature = 0;
            [DataMember] public string[] stop = { "\n", "\r", "<|fim_pad|>", "<|endoftext|>", "<|im_end|>", "<|file_sep|>" };
        }
        [DataContract]
        private sealed class GenerateResponse
        {
            [DataMember] public string response { get; set; }
            [DataMember] public string error { get; set; }
        }
    }
}
