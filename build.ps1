$QtDir = "C:\Qt\6.11.0\mingw_64"
$MingwDir = "C:\msys64\ucrt64"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition

$env:Qt6_DIR = "$QtDir\lib\cmake\Qt6"
$env:PATH = "$MingwDir\bin;$QtDir\bin;$env:PATH"

Write-Host "=== Configuring CMake ==="
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="$MingwDir/bin/g++.exe" -S $ProjectRoot -B "$ProjectRoot/build"

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "=== Building ==="
mingw32-make -C "$ProjectRoot/build"

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "=== Deploying Qt DLLs ==="
& "$QtDir\bin\windeployqt6.exe" --no-translations --no-system-d3d-compiler "$ProjectRoot/build/QuoridorGame.exe"

Write-Host "=== Done ==="
Write-Host "Run with: $ProjectRoot\build\QuoridorGame.exe"
