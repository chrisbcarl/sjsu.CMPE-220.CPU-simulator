# if there is a poetry env to invoke
# poetry config virtualenvs.in-project false --local
# $poetry_env_list=$(poetry env list)
# & "$env:LOCALAPPDATA/pypoetry/Cache/virtualenvs/$poetry_env_list/Scripts/Activate.ps1"
# poetry config virtualenvs.in-project true --local
& "$PSScriptRoot\..\..\.venv\scripts\activate.ps1"

# EXAMPLE
Write-Host -ForegroundColor Green "echo current python..."
python -c "import sys; print(sys.executable)"

if ($env:USERNAME -eq "chris") {
    Write-Host -ForegroundColor Green "doc-watch for '$($env:USERNAME)'..."
    $docs = "C:\users\chris\src\chriscarl.tools.documents"
    & "$docs\.venv\scripts\doc-watch.cmd" dirs ./ --md-table-pretty
}
