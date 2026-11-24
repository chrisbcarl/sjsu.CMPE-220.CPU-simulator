<#
Description:
    Poor man's makefile. works though!

Dependencies:
    choco install mingw -y

Example:
    gc+-build.ps1 g++ ./src /bin/main
    gc+-build.ps1 -e g++ -main ./src/main.cpp -o /bin/main

Updates:
    2026-11-23 - Chris Carl - gc+-build.ps1 - runs all test cases in ./test
    2026-11-22 - Chris Carl - gc+-build.ps1 - added a -I so tasks.json can forcefully pipe ./src in there

#>
[CmdletBinding()]
param (
    [Parameter()][string]$e="g++",
    [Parameter()][string]$d=([IO.Path]::GetFullPath("$PSScriptRoot/../../") | Resolve-Path -Relative),
    [Parameter()][string]$I=("$PSScriptRoot/../../src" | Resolve-Path -Relative),
    [Parameter(Mandatory=$true)][string]$main,
    [Parameter(Mandatory=$true)][string]$o
)
$Debug = $PSBoundParameters.ContainsKey('Debug') -and $PSBoundParameters['Debug'].IsPresent

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
$HFiles = (@($I, $d) | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.h | Where-Object {-Not $_.FullName.Contains("ignoreme")} } | Resolve-Path -Relative)
# (Get-ChildItem -Path $d -Recurse -Directory | Resolve-Path -Relative | ForEach-Object {'-I' + $_})
$Includes = $HFiles | ForEach-Object { Split-Path $_ -Parent } | Sort-Object -Unique | ForEach-Object {'-I' + $_}
$Sources = (@($I, $d) | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.c,*.cc,*.cpp | Where-Object {-Not $_.FullName.Contains("ignoreme")}} | Resolve-Path -Relative)
$Tests = ("$d/test" | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.c,*.cc,*.cpp | Where-Object {-Not $_.FullName.Contains("ignoreme")}} | Resolve-Path -Relative)
# scan sources for int main, omit them. we're only using the main file's main. everything else would be in conflict.
$Sources = $Sources | Where-Object { (-Not (Select-String -Path $_ -Pattern "int main\(")) -and (-Not $Tests.Contains($_)) -and ($_ -ne $main)  } | Select-Object -Unique
# Write-Host $Sources
# exit 0


if ($Debug) {
    Write-Host "DEBUG: Includes - '$Includes'"
    Write-Host "DEBUG: Tests - '$Tests'"
    Write-Host "DEBUG: Sources - '$Sources'"
}

# build tests
for ($itr = 0; $itr -lt $Tests.Count; $itr++) {
    Write-Host -ForegroundColor Cyan "Test ($($itr + 1)/$($Tests.Count))"
    $c_file = $Tests[$itr]
    $filename = [IO.Path]::GetFileNameWithoutExtension($c_file)
    $output = [IO.path]::GetFullPath("$OutputDirpath/$filename.exe")
    if (Test-Path $output -ErrorAction SilentlyContinue) {Remove-Item -Path $output}

    # -g for GDB
    $ArgumentList = @("-g") + @($Includes) + @($Sources) + @($c_file) + @('-fdiagnostics-color=always', '-g', '-o', $output)
    Write-Host -ForegroundColor Cyan "     $e $ArgumentList"
    $res = Start-Process -FilePath $e -ArgumentList $ArgumentList -NoNewWindow -Wait -PassThru
    if (0 -ne $res.ExitCode) {exit $res.ExitCode}
    if (-not (Test-Path $output -ErrorAction SilentlyContinue)) {exit 1;}

    & $output
    if (0 -ne $LASTEXITCODE) {exit $LASTEXITCODE}
    Write-Host ""
}

# build main
# -g for GDB
Write-Host -ForegroundColor Cyan "Main"
if (Test-Path $output -ErrorAction SilentlyContinue) {Remove-Item -Path $output}
$ArgumentList = @("-g") + @($Includes) + @($Sources) + @($main) + @('-fdiagnostics-color=always', '-g', '-o', $o)
Write-Host -ForegroundColor Cyan "    $e $ArgumentList"
$res = Start-Process -FilePath "$e" -ArgumentList $ArgumentList -NoNewWindow -Wait -PassThru
if (-not (Test-Path $o -ErrorAction SilentlyContinue)) {exit 1;}
exit $res.ExitCode
