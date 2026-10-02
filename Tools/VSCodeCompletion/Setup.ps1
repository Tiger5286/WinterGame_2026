param([switch]$SkipVsix)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$model = 'qwen2.5-coder:1.5b-base'

try {
    Write-Host 'Local Completion setup for Visual Studio 2022' -ForegroundColor Cyan
    Write-Host 'This installs Ollama if needed and downloads a local model (~1 GB). No API key is needed.'
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Visual Studio Installer was not found.' }
    $vsPath = & $vswhere -latest -products '*' -version '[17.14,18.0)' -property installationPath
    if (-not $vsPath) { throw 'Visual Studio 2022 17.14 or later is required.' }

    $ollamaCommand = Get-Command ollama.exe -ErrorAction SilentlyContinue
    $ollama = if ($ollamaCommand) { $ollamaCommand.Source } else { Join-Path $env:LOCALAPPDATA 'Programs/Ollama/ollama.exe' }
    if (-not (Test-Path -LiteralPath $ollama)) {
        if (-not (Get-Command winget.exe -ErrorAction SilentlyContinue)) {
            throw 'Install Ollama from https://ollama.com/download/windows then run Setup.cmd again.'
        }
        & winget.exe install --id Ollama.Ollama --exact --source winget --accept-package-agreements --accept-source-agreements
        if ($LASTEXITCODE -ne 0) { throw 'Ollama installation failed. Install it from https://ollama.com/download/windows and try again.' }
        if (-not (Test-Path -LiteralPath $ollama)) { throw 'Ollama was installed in a custom folder. Reopen a terminal and run Setup.ps1 again.' }
    }

    # Only the local base model is used. A server started here has cloud features disabled.
    $env:OLLAMA_HOST = '127.0.0.1:11434'
    $env:OLLAMA_NO_CLOUD = '1'
    $running = $false
    try { $null = Invoke-RestMethod 'http://127.0.0.1:11434/api/version' -TimeoutSec 2; $running = $true } catch { }
    if (-not $running) {
        Start-Process -FilePath $ollama -ArgumentList 'serve' -WindowStyle Hidden
        for ($attempt = 0; $attempt -lt 30; $attempt++) {
            Start-Sleep -Seconds 1
            try { $null = Invoke-RestMethod 'http://127.0.0.1:11434/api/version' -TimeoutSec 2; $running = $true; break } catch { }
        }
        if (-not $running) { throw 'Ollama did not start. Launch Ollama manually and try again.' }
    }

    & $ollama pull $model
    if ($LASTEXITCODE -ne 0) { throw 'Model download failed. Check the network connection and retry.' }
    Write-Host 'Testing local code generation (first load may take a minute)...'
    $body = @{
        model = $model; raw = $true; stream = $false; keep_alive = '2m'
        prompt = "<|fim_prefix|>int Add(int a, int b) { return<|fim_suffix|>; }<|fim_middle|>"
        options = @{ num_predict = 32; num_ctx = 4096; temperature = 0; stop = @("`n", '<|endoftext|>') }
    } | ConvertTo-Json -Depth 4
    $result = Invoke-RestMethod 'http://127.0.0.1:11434/api/generate' -Method Post -ContentType 'application/json' -Body $body -TimeoutSec 120
    if ([string]::IsNullOrWhiteSpace($result.response)) { throw 'The local model returned no completion. Try the connection test in Visual Studio.' }
    Write-Host ('Model is ready. Example completion: ' + $result.response) -ForegroundColor Green

    if (-not $SkipVsix) {
        $vsix = Join-Path $PSScriptRoot 'artifacts/LocalCompletion.vsix'
        if (-not (Test-Path -LiteralPath $vsix)) { $vsix = Join-Path $PSScriptRoot 'bin/Release/net472/LocalCompletion.vsix' }
        if (-not (Test-Path -LiteralPath $vsix)) { throw 'LocalCompletion.vsix was not found. Run Build.ps1 first.' }
        $installer = Join-Path $vsPath 'Common7/IDE/VSIXInstaller.exe'
        Write-Host 'Save your work and close Visual Studio, then complete the VSIX installation dialog.' -ForegroundColor Yellow
        $install = Start-Process -FilePath $installer -ArgumentList ('"' + $vsix + '"') -Wait -PassThru
        if ($install.ExitCode -ne 0) { throw ('VSIX installation did not complete. Exit code: ' + $install.ExitCode) }
    }
    Write-Host 'Ready. Open a C++ file in VS2022. Ctrl+Alt+Enter: suggest, Tab: accept, Esc: dismiss.' -ForegroundColor Green
} catch {
    Write-Error $_
    exit 1
}
