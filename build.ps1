# Builds H5_DLL.sln (Release|x86) and deploys it as um.dll through .sync-dll.ps1.

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

$msbuild = $null
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $msbuild = & $vswhere -latest -prerelease -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
}
if (-not $msbuild) { $msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" }
if (-not (Test-Path $msbuild)) { throw "MSBuild not found: $msbuild" }

& $msbuild H5_DLL.sln -p:Configuration=Release -p:Platform=x86 -m -v:minimal -nologo
if ($LASTEXITCODE -ne 0) { throw "Build failed (MSBuild exit code $LASTEXITCODE)" }

& "$PSScriptRoot\.sync-dll.ps1"
# Robocopy: exit codes below 8 mean success
if ($LASTEXITCODE -ge 8) { throw "Sync failed (Robocopy exit code $LASTEXITCODE)" }

Write-Host "Done"
exit 0
