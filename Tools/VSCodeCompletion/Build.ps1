$ErrorActionPreference = 'Stop'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsPath = & $vswhere -latest -products '*' -version '[17.14,18.0)' -requires Microsoft.Component.MSBuild -property installationPath
if (-not $vsPath) { throw 'Visual Studio 2022 17.14+ with MSBuild is required.' }
& (Join-Path $vsPath 'MSBuild/Current/Bin/MSBuild.exe') (Join-Path $PSScriptRoot 'LocalCompletion.csproj') /restore /t:Build /p:Configuration=Release /v:minimal /nologo
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
# Verify that the menu resource referenced by ProvideMenuResource is packaged.
$assembly = [System.Reflection.Assembly]::LoadFile((Join-Path $PSScriptRoot 'bin/Release/net472/LocalCompletion.dll'))
$resourceFound = $false
foreach ($name in $assembly.GetManifestResourceNames()) {
    if ($name.EndsWith('.resources')) {
        $reader = New-Object System.Resources.ResourceReader($assembly.GetManifestResourceStream($name))
        try { foreach ($entry in $reader) { if ($entry.Key -eq 'Commands.CTMENU') { $resourceFound = $true } } }
        finally { $reader.Dispose() }
    }
}
if (-not $resourceFound) { throw 'The command menu resource was not embedded.' }
$artifacts = Join-Path $PSScriptRoot 'artifacts'
New-Item -ItemType Directory -Path $artifacts -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'bin/Release/net472/LocalCompletion.vsix') -Destination $artifacts
Write-Host ('Built: ' + (Join-Path $artifacts 'LocalCompletion.vsix'))
