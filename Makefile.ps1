<#
2026-11-24 - Chris Carl - Makefile.ps1 - make it clearly analogous to Makefile, unfortunately re-compiles all files

#>

[CmdletBinding()]
param ()
$Debug = $PSBoundParameters.ContainsKey('Debug') -and $PSBoundParameters['Debug'].IsPresent

# Run all in ./test and ./src/main.c in that order
$files = ((Get-ChildItem -Path "$PSScriptRoot/test" -Recurse -File -Include *.c,*.cc,*.cpp | Where-Object {-Not $_.FullName.Contains("ignoreme")} | Resolve-Path -Relative)) + @("$PSScriptRoot/src/main.c")
for ($i = 0; $i -lt $files.Count; $i++) {
    $filepath = $files[$i]
    $filename = [IO.Path]::GetFileNameWithoutExtension($filepath)
    Write-Host -ForegroundColor Cyan "($($i + 1)/$($files.Count)) Compiling '$filepath'"
    & .vscode\tasks\gc+-build.ps1 `
        -exe "gcc" `
        -main $filepath `
        -out "$PSScriptRoot/bin/$filename" `
        -src "$PSScriptRoot/src" -Debug:$Debug
    if (0 -ne $LASTEXITCODE) {exit $LASTEXITCODE}
    Write-Host -ForegroundColor Cyan "    running '$filepath'"
    & "$PSScriptRoot/bin/$filename"
    if (0 -ne $LASTEXITCODE) {exit $LASTEXITCODE}
    Write-Host ""
}
