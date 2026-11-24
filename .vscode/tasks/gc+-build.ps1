<#
Description:
    Poor man's makefile. works though!

Dependencies:
    choco install mingw -y

Example:
    gc+-build.ps1 g++ ./src /bin/main
    gc+-build.ps1 -e g++ -main ./src/main.cpp -o /bin/main

Updates:
    2026-11-22 - Chris Carl - gc+-build.ps1 - added a -I so tasks.json can forcefully pipe ./src in there

#>

[CmdletBinding()]
param (
    [Parameter()][string]$e="g++",
    [Parameter(Mandatory=$true)][string]$d="",
    [Parameter(Mandatory=$true)][string]$I="",
    [Parameter(Mandatory=$true)][string]$main,
    [Parameter(Mandatory=$true)][string]$o
)

$main = $main | Resolve-Path -Relative
if (-Not $d) {
    $d = ([IO.Path]::GetDirectoryName($main)) | Resolve-Path -Relative
}

if ($Debug) {
    Write-Host "DEBUG: exe - $e"
    Write-Host "DEBUG: source/header directory - '$d'"
    Write-Host "DEBUG: main entry point - '$main'"
    Write-Host "DEBUG: output filepath - '$o'"
}


# clean
if (Test-Path $o -ErrorAction SilentlyContinue) {
    Remove-Item $o
}
$OutputDirpath = ([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($o)))
Write-Host -ForegroundColor Cyan "mkdir '$OutputDirpath'"
New-Item -Path $OutputDirpath -ItemType Directory -ErrorAction SilentlyContinue

# args
$HFiles = (@($I, $d) | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.h} | Resolve-Path -Relative)
# (Get-ChildItem -Path $d -Recurse -Directory | Resolve-Path -Relative | ForEach-Object {'-I' + $_})
$Includes = $HFiles | ForEach-Object { Split-Path $_ -Parent } | Sort-Object -Unique | ForEach-Object {'-I' + $_}
$Sources = (@($I, $d) | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.c,*.cc,*.cpp} | Resolve-Path -Relative)
$main = $main | Resolve-Path -Relative
# scan sources for int main, omit them. we're only using the main file's main. everything else would be in conflict.
$Sources = $Sources | Where-Object { -Not (Select-String -Path $_ -Pattern "int main\(") }


# -g for GDB
$ArgumentList = @("-g") + @($main) + @($Sources) + @($Includes) + @('-fdiagnostics-color=always', '-g', '-o', $o)

if ($Debug) {
    Write-Host "DEBUG: Includes - $Includes"
    Write-Host "DEBUG: Sources - $Sources"
}

# build
Write-Host -ForegroundColor Cyan "$e $ArgumentList"
Start-Process -FilePath $e -ArgumentList $ArgumentList -NoNewWindow -Wait

exit $LASTEXITCODE
