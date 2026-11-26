# Arma el .zip del release (fase 13): los dos ejecutables de Release y el LEEME.txt.
#
# Uso (desde la raíz del repo, después de `cmake --build build --config Release`):
#   powershell -File scripts/empaquetar-release.ps1 -Version v1.0.0
# Deja dist/tp-chess-<version>-windows-x64.zip. dist/ no se versiona.
param(
    [Parameter(Mandatory = $true)][string]$Version
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$binaries = Join-Path $root 'build/app/Release'
$name = "tp-chess-$Version-windows-x64"
$staging = Join-Path $root "dist/$name"
$zip = Join-Path $root "dist/$name.zip"

foreach ($exe in 'chess_console.exe', 'chess_raylib.exe') {
    if (-not (Test-Path (Join-Path $binaries $exe))) {
        throw "Falta $exe en ${binaries}: compilar con cmake --build build --config Release"
    }
}

if (Test-Path $staging) { Remove-Item -Recurse -Force $staging }
if (Test-Path $zip) { Remove-Item -Force $zip }
New-Item -ItemType Directory -Force $staging | Out-Null

Copy-Item (Join-Path $binaries 'chess_console.exe'), (Join-Path $binaries 'chess_raylib.exe') $staging
Copy-Item (Join-Path $root 'docs/release/LEEME.txt') $staging

Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zip
Write-Output "Release empaquetado en $zip"
