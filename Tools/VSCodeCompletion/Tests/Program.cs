using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using LocalCompletion;

int checks = 0;
void Check(bool condition, string name)
{
    if (!condition) throw new Exception("FAIL: " + name);
    checks++;
    Console.WriteLine("PASS: " + name);
}
void Reject(Action action, string name)
{
    try { action(); } catch (ArgumentException) { Check(true, name); return; }
    throw new Exception("FAIL: " + name);
}

Check(CompletionCore.LocalEndpoint("http://localhost:11434").AbsolutePath == "/api/generate", "Local endpoint");
Check(CompletionCore.LocalEndpoint("http://127.0.0.1:11434").IsLoopback, "IPv4 loopback");
Check(CompletionCore.LocalEndpoint("http://[::1]:11434").IsLoopback, "IPv6 loopback");
foreach (string bad in new[] { "https://api.openai.com", "http://192.168.1.2:11434", "file:///test", "http://localhost:11434/else", "http://a:b@localhost:11434", "http://localhost:11434?x=1" })
    Reject(() => CompletionCore.LocalEndpoint(bad), "Reject " + bad);
Check(CompletionCore.FirstLine(" a + b;\r\nmalicious hidden edit") == " a + b;", "Insert only displayed line");
Check(CompletionCore.FirstLine("\treturn x;  ") == "\treturn x;", "Preserve indentation");
Check(CompletionCore.FirstLine(" x;<|endoftext|>") == " x;", "Remove special tokens");
Check(CompletionCore.FirstLine("```cpp") == "", "Reject Markdown fence");
Check(CompletionCore.FirstLine("\nreturn x;") == "", "Do not silently accept hidden next line");
Check(CompletionCore.FirstLine("\u0001abc") == "", "Reject control characters");
Check(!CompletionCore.IsCurrent(1, 2, 10, 10), "Reject response after edit");
Check(!CompletionCore.IsCurrent(1, 1, 10, 11), "Reject response after caret move");
Check(CompletionCore.IsCurrent(1, 1, 10, 10), "Accept current response");
Reject(() => new CompletionSettings { Model = "model:cloud" }.Validate(), "Reject cloud model");
Reject(() => new CompletionSettings { DelayMilliseconds = 0 }.Validate(), "Reject busy-loop delay");

// 実際のHTTPクライアントをローカルの偽Ollamaに接続して検証する。
using var server = new FakeOllama();
var settings = new CompletionSettings { Endpoint = server.Endpoint, TimeoutSeconds = 5 };
var generated = OllamaClient.GenerateAsync(settings, "int 合計 =", ";\n", CancellationToken.None);
using (var call = await server.AcceptAsync())
{
    using var json = JsonDocument.Parse(call.Body);
    Check(json.RootElement.GetProperty("model").GetString() == "qwen2.5-coder:1.5b-base", "Default local model");
    Check(json.RootElement.GetProperty("prompt").GetString() == "<|fim_prefix|>int 合計 =<|fim_suffix|>;\n<|fim_middle|>", "UTF-8 FIM context");
    Check(json.RootElement.GetProperty("raw").GetBoolean() && !json.RootElement.GetProperty("stream").GetBoolean(), "Raw non-streaming request");
    await call.ReplyAsync(200, "{\"response\":\" a + b;\\nignored\"}");
}
Check(await generated == " a + b;", "Parse Ollama response");

var missing = OllamaClient.GenerateAsync(settings, "x", "", CancellationToken.None);
using (var call = await server.AcceptAsync()) await call.ReplyAsync(404, "{}");
try { await missing; throw new Exception("Expected 404 failure"); }
catch (InvalidOperationException ex) { Check(ex.Message.Contains("ollama pull"), "Missing model has actionable error"); }

using (var cts = new CancellationTokenSource())
{
    var cancelled = OllamaClient.GenerateAsync(settings, "x", "", cts.Token);
    using var call = await server.AcceptAsync();
    cts.Cancel();
    try { await cancelled; throw new Exception("Expected cancellation"); }
    catch (OperationCanceledException) { Check(true, "Cancel in-flight request"); }
}

var timeout = OllamaClient.GenerateAsync(settings, "x", "", CancellationToken.None);
using (var call = await server.AcceptAsync())
{
    try { await timeout; throw new Exception("Expected timeout"); }
    catch (OperationCanceledException) { Check(true, "HTTP timeout"); }
}
// キャンセルやタイムアウト後にも同時実行制限が解放されていること。
var recovered = OllamaClient.GenerateAsync(settings, "x", "", CancellationToken.None);
using (var call = await server.AcceptAsync()) await call.ReplyAsync(200, "{\"response\":\" = 1;\"}");
Check(await recovered == " = 1;", "Request recovers after cancellation and timeout");
Console.WriteLine($"All {checks} checks passed.");

if (args.Contains("--live"))
{
    var watch = System.Diagnostics.Stopwatch.StartNew();
    string completion = await OllamaClient.GenerateAsync(new CompletionSettings(),
        "int Add(int a, int b)\n{\n    return", "\n}\n", CancellationToken.None);
    Check(!string.IsNullOrWhiteSpace(completion), "Live Ollama returns a completion");
    Console.WriteLine($"Live completion ({watch.ElapsedMilliseconds} ms): return{completion}");
}

sealed class FakeOllama : IDisposable
{
    private readonly TcpListener listener = new(IPAddress.Loopback, 0);
    public string Endpoint { get; }
    public FakeOllama()
    {
        listener.Start();
        Endpoint = "http://127.0.0.1:" + ((IPEndPoint)listener.LocalEndpoint).Port;
    }
    public async Task<Call> AcceptAsync()
    {
        using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(10));
        var client = await listener.AcceptTcpClientAsync(timeout.Token);
        var stream = client.GetStream();
        var headers = new List<byte>();
        var one = new byte[1];
        while (true)
        {
            if (await stream.ReadAsync(one, timeout.Token) == 0) throw new Exception("Unexpected EOF");
            headers.Add(one[0]);
            if (headers.Count >= 4 && headers.TakeLast(4).SequenceEqual(new byte[] { 13, 10, 13, 10 })) break;
        }
        var lines = Encoding.ASCII.GetString(headers.ToArray()).Split("\r\n");
        int length = int.Parse(lines.First(l => l.StartsWith("Content-Length:", StringComparison.OrdinalIgnoreCase)).Split(':')[1]);
        var body = new byte[length];
        await stream.ReadExactlyAsync(body, timeout.Token);
        return new Call(client, Encoding.UTF8.GetString(body));
    }
    public void Dispose() => listener.Stop();
    public sealed class Call(TcpClient client, string body) : IDisposable
    {
        public string Body { get; } = body;
        public async Task ReplyAsync(int code, string text)
        {
            byte[] bodyBytes = Encoding.UTF8.GetBytes(text);
            byte[] headers = Encoding.ASCII.GetBytes($"HTTP/1.1 {code} Result\r\nContent-Type: application/json\r\nContent-Length: {bodyBytes.Length}\r\nConnection: close\r\n\r\n");
            await client.GetStream().WriteAsync(headers);
            await client.GetStream().WriteAsync(bodyBytes);
        }
        public void Dispose() => client.Dispose();
    }
}
