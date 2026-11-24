[CmdletBinding()]
param ()
$Debug = $PSBoundParameters.ContainsKey('Debug') -and $PSBoundParameters['Debug'].IsPresent

.vscode\tasks\gc+-build.ps1 -e "gcc" -main "$PSScriptRoot/src/main.c" -o "$PSScriptRoot/bin/main" -d "$PSScriptRoot" -I "$PSScriptRoot/src" -Debug:$Debug
# & bin/main
