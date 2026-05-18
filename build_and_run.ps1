param(
    [string]$CocosRoot = "D:\cocos2d_tools\cocos2d-x-3.17.2\cocos2d-x-cocos2d-x-3.17.2",
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug",
    [switch]$Run
)

$ErrorActionPreference = "Stop"

$cmake = "C:\Program Files\CMake\bin\cmake.exe"
if (!(Test-Path $cmake)) {
    $cmake = "cmake"
}

& $cmake -S $PSScriptRoot -B "$PSScriptRoot\build" `
    -G "Visual Studio 17 2022" -A Win32 `
    "-DCOCOS2DX_ROOT_PATH=$CocosRoot"

& $cmake --build "$PSScriptRoot\build" --config $Config --target PokerMatchTest -j 4

if ($Run) {
    & "$PSScriptRoot\build\bin\PokerMatchTest\$Config\PokerMatchTest.exe"
}
