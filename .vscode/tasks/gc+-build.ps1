<#
Description:
    Poor man's makefile. works though!

Dependencies:
    choco install mingw -y

Example:
    gc+-build.ps1 g++ ./src /bin/main
    gc+-build.ps1 -e g++ -main ./src/main.cpp -o /bin/main
    & .vscode\tasks\gc+-build.ps1 `
        -exe "gcc" -src "./src" ` -test "./test"
        -main ".\src\main.c" -o "./bin/main" -Debug

    & .vscode\tasks\gc+-build.ps1 `
        -exe "gcc" -src "./src" ` -test "./test" `
        -main ".\test\isa\test_isa.c" -o "./bin/isa/test_isa" -Debug

Updates:
    2026-11-24 - Chris Carl - gc+-build.ps1 - run ONLY ONE FILE, using the rest of the sources correctly and ignoring any that have void/int main other than the one you wanted
    2026-11-23 - Chris Carl - gc+-build.ps1 - runs all test cases in ./test
    2026-11-22 - Chris Carl - gc+-build.ps1 - added a -I so tasks.json can forcefully pipe ./src in there

#>
[CmdletBinding()]
param (
    [Parameter()][string]$exe="g++",
    [Parameter(Mandatory=$true)][string]$src,
    [Parameter(Mandatory=$true)][string]$main,
    [Parameter(Mandatory=$true)][string]$out
)
$Debug = $PSBoundParameters.ContainsKey('Debug') -and $PSBoundParameters['Debug'].IsPresent

$main = $main | Resolve-Path -Relative
if ($Debug) {
    Write-Host "DEBUG: cwd - $($PWD.Path)"
    Write-Host "& $PSScriptRoot\gc+-build.ps1 -exe `"$exe`" -src `"$src`" -test `"$test`" -main `"$main`" -out `"$out`""
}


# clean
$OutputDirpath = ([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($out)))
# Write-Host -ForegroundColor Cyan "mkdir '$OutputDirpath'"
New-Item -Path $OutputDirpath -ItemType Directory -ErrorAction SilentlyContinue

# gcc/++ args
$Sources = ($src | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.c,*.cc,*.cpp | Where-Object {-Not $_.FullName.Contains("ignoreme")}} | Resolve-Path -Relative)
$Sources = $Sources | Where-Object { (-Not (Select-String -Path $_ -Pattern "(int|void) main\(")) -and ($_ -ne $main)  } | Select-Object -Unique
$Headers = ($src | ForEach-Object {Get-ChildItem -Path $_ -Recurse -File -Include *.h | Where-Object {-Not $_.FullName.Contains("ignoreme")} } | Resolve-Path -Relative)
$Includes = $Headers | ForEach-Object { Split-Path $_ -Parent } | Sort-Object -Unique | ForEach-Object {'-I' + $_}

if ($Debug) {
    Write-Host "DEBUG: Headers      $Headers"
    Write-Host "DEBUG: Includes    $Includes"
    Write-Host "DEBUG: Sources     $Sources"
}

# build main
# -g for GDB
if (Test-Path $out -ErrorAction SilentlyContinue) {Remove-Item -Path $out}
$ArgumentList = @("-g") + @($Includes) + @($Sources) + @($main) + @('-fdiagnostics-color=always', '-g', '-o', $out)
Write-Host -ForegroundColor Cyan "$exe $ArgumentList"
$res = Start-Process -FilePath "$exe" -ArgumentList $ArgumentList -NoNewWindow -Wait -PassThru
exit $res.ExitCode
