$releases = Invoke-RestMethod "https://api.github.com/repos/frypan05/Volt/releases"
$total = 0
foreach ($r in $releases) {
    Write-Host "`nRelease: $($r.tag_name)"
    foreach ($a in $r.assets) {
        Write-Host "  $($a.name): $($a.download_count)"
        $total += $a.download_count
    }
}
Write-Host "`nTotal downloads: $total"
