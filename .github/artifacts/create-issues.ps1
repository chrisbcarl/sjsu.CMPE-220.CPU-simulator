<#
2026-11-25 - Chris Carl - create-issues.ps1 - small wrapper around creating issues which is another wrapper around another project i've done. got tired of changing text by hand...
#>


[CmdletBinding()]
param (
    [Parameter(Mandatory=$true)][string]$Issue
)

$cwd = Get-Location
try {
    Set-Location "../"
    if (-Not (Test-Path "bootstrap" -ErrorAction SilentlyContinue)) {
        git clone "https://github.com/chrisbcarl/bootstrap.git"
    }
    if (-Not (Test-Path "sjsu.CMPE-220.CPU-simulator/.github/artifacts/issues-$Issue.csv" -ErrorAction SilentlyContinue)) {
        Write-Warning "-Issue '.github/artifacts/issues-$Issue.csv' does not exist!"
        exit 1
    }
    python "bootstrap/apps/github-issue-creator/github-issue-creator.py" `
        csv "sjsu.CMPE-220.CPU-simulator/.github/artifacts/issues-$Issue.csv" `
        --issue-template-filepath "sjsu.CMPE-220.CPU-simulator/.github/artifacts/issue-template.md" `
        --repo "chrisbcarl/sjsu.CMPE-220.CPU-simulator"
} finally {
    Set-Location $cwd
}
