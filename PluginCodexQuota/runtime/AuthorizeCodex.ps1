$binDir = Join-Path $env:LOCALAPPDATA 'OpenAI\Codex\bin'
$candidates = Get-ChildItem -LiteralPath $binDir -Recurse -Filter 'codex.exe' -ErrorAction Stop |
    Sort-Object LastWriteTime -Descending
if ($candidates.Count -eq 0) { throw 'Codex CLI was not found.' }

# This opens the official Codex login flow. No password, cookie, or token is handled by this script.
& $candidates[0].FullName login
