$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSCommandPath
$reader = Join-Path $root 'read-codex-quota.cjs'
$state = Join-Path $root 'quota-state.json'
$log = Join-Path $root 'quota-refresh.log'
$mutex = [Threading.Mutex]::new($false, 'Local\CodexQuotaTaskbar.Refresh')
$lockTaken = $false
function Write-State([object]$value) {
    $temporary = "$state.tmp"
    Set-Content -LiteralPath $temporary -Value ($value | ConvertTo-Json) -Encoding utf8
    Move-Item -LiteralPath $temporary -Destination $state -Force
}
try {
    $lockTaken = $mutex.WaitOne(0)
    if (-not $lockTaken) { exit 0 }
    $bundledNode = Join-Path $env:LOCALAPPDATA 'OpenAI\Codex\bin\node.exe'
    $node = if (Test-Path -LiteralPath $bundledNode) { $bundledNode } else { (Get-Command node.exe -ErrorAction Stop).Source }
    $json = (& $node $reader 2>$null | Select-Object -Last 1)
    $result = $json | ConvertFrom-Json
    if ($result.status -eq 'ok') {
        $property = { param($object, $camel, $snake) $camelValue = $object.PSObject.Properties[$camel].Value; if ($null -ne $camelValue) { return $camelValue }; return $object.PSObject.Properties[$snake].Value }
        $toQuota = { param($limit) $used = & $property $limit 'usedPercent' 'used_percent'; [math]::Round([math]::Max(0, [math]::Min(100, 100 - [double]$used))) }
        $toTime = { param($limit) $unix = & $property $limit 'resetsAt' 'resets_at'; [DateTimeOffset]::FromUnixTimeSeconds([int64]$unix).ToLocalTime().ToString('HH:mm') }; $toDate = { param($limit) $unix = & $property $limit 'resetsAt' 'resets_at'; [DateTimeOffset]::FromUnixTimeSeconds([int64]$unix).ToLocalTime().ToString('MM-dd') }
        $limits = @($result.primary, $result.secondary) | Where-Object { $null -ne $_ }
        $duration = { param($limit) [int](& $property $limit 'windowDurationMins' 'window_duration_mins') }
        $weekly = $limits | Where-Object { (& $duration $_) -ge 10080 } | Select-Object -First 1
        $shortWindow = $limits | Where-Object { $minutes = & $duration $_; $minutes -gt 0 -and $minutes -lt 10080 } | Select-Object -First 1
        if ($null -eq $weekly) { throw 'Codex did not return a weekly quota window.' }
        Write-State ([ordered]@{ hasFiveHour = ($null -ne $shortWindow); fiveHourRemaining = if ($null -ne $shortWindow) { & $toQuota $shortWindow } else { 0 }; fiveHourReset = if ($null -ne $shortWindow) { & $toTime $shortWindow } else { '' }; weeklyRemaining = & $toQuota $weekly; weeklyReset = & $toDate $weekly; updatedAt = Get-Date -Format 'MM-dd HH:mm:ss'; status = 'ok' })
        "$(Get-Date -Format s) ok: windows=$(@($limits).Count), short=$($null -ne $shortWindow)" | Add-Content -LiteralPath $log -Encoding utf8
    } elseif ($result.status -eq 'needs-login') {
        $previous = if (Test-Path $state) { Get-Content -Raw $state | ConvertFrom-Json } else { [pscustomobject]@{} }
        $previous | Add-Member -NotePropertyName status -NotePropertyValue 'needs-login' -Force
        Write-State $previous
        "$(Get-Date -Format s) needs-login" | Add-Content -LiteralPath $log -Encoding utf8
    } else {
        "$(Get-Date -Format s) $($result.status): $($result.message)" | Add-Content -LiteralPath $log -Encoding utf8
    }
} catch {
    "$(Get-Date -Format s) exception: $($_.Exception.Message)" | Add-Content -LiteralPath $log -Encoding utf8
} finally {
    if ($lockTaken) { $mutex.ReleaseMutex() }
    $mutex.Dispose()
}

