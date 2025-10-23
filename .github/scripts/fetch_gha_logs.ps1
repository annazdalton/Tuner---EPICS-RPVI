param(
    [string]$Owner = 'agandav',
    [string]$Repo = 'Tuner---EPICS-RPVI',
    [string]$Branch = 'Sai',
    [int]$PollIntervalSec = 10,
    [int]$MaxAttempts = 60
)

Write-Host ('Querying GitHub Actions for {0}/{1} branch {2}' -f $Owner, $Repo, $Branch)

$attempt = 0
$run = $null
while ($attempt -lt $MaxAttempts) {
    $attempt++
    try {
        $url = "https://api.github.com/repos/$Owner/$Repo/actions/runs?branch=$Branch&per_page=5"
        $runs = Invoke-RestMethod -Uri $url -UseBasicParsing -ErrorAction Stop
    } catch {
    Write-Host ('Attempt {0}: Failed to query GitHub API: {1}' -f $attempt, $_)
        Start-Sleep -Seconds $PollIntervalSec
        continue
    }
    if (-not $runs.workflow_runs -or $runs.workflow_runs.Count -eq 0) {
    Write-Host ('Attempt {0}: no runs found yet' -f $attempt)
        Start-Sleep -Seconds $PollIntervalSec
        continue
    }
    $run = $runs.workflow_runs[0]
    Write-Host ('Attempt {0}: Found run id {1} - status: {2} - conclusion: {3}' -f $attempt, $run.id, $run.status, $run.conclusion)
    if ($run.status -eq 'completed') { break }
    Start-Sleep -Seconds $PollIntervalSec
}

if (-not $run) {
    Write-Error "No run found after $MaxAttempts attempts"
    exit 2
}

if ($run.status -ne 'completed') {
    Write-Error "Run found but not completed (status=$($run.status)). Exiting."
    exit 3
}

$logsUrl = $run.logs_url
Write-Host "Downloading logs from $logsUrl"
try {
    Invoke-WebRequest -Uri $logsUrl -OutFile logs.zip -UseBasicParsing -ErrorAction Stop
} catch {
    Write-Error "Failed to download logs.zip: $_"
    exit 4
}

$dest = Join-Path $PSScriptRoot '..\..\gha-logs'
if (Test-Path $dest) { Remove-Item $dest -Recurse -Force }
New-Item -ItemType Directory -Path $dest | Out-Null

try {
    Expand-Archive -LiteralPath logs.zip -DestinationPath $dest -Force
} catch {
    Write-Error "Failed to extract logs.zip: $_"
    exit 5
}

Write-Host "Extracted logs to: $dest"

$files = Get-ChildItem -Path $dest -Recurse | Where-Object { $_.Name -match 'teensy|native|build-and-test|build_and_test' }
if ($files.Count -eq 0) { $files = Get-ChildItem -Path $dest -Recurse }

foreach ($f in $files) {
    Write-Host "\n==== $($f.FullName) ====\n"
    try { Get-Content $f.FullName -Raw } catch { Write-Host "(failed to read $($f.FullName))" }
}

Write-Host "Done"
