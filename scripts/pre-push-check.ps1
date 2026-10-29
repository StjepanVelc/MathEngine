# Provjera prije pusha: sintaksa JS-a, JSON banke, web validacija, DOM test i (ako postoji build) ctest.
# Pokretanje: powershell -File scripts/pre-push-check.ps1
$ErrorActionPreference = "Continue"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
$failed = 0

function Step($name, [scriptblock]$body) {
	Write-Host "==> $name"
	& $body
	if ($LASTEXITCODE -ne 0) { Write-Host "FAIL: $name" -ForegroundColor Red; $script:failed++ }
}

Step "node --check web/*.js" {
	$code = 0
	Get-ChildItem web -Filter *.js | Where-Object { $_.Name -notmatch '^aksiomat\.js$' } | ForEach-Object {
		node --check $_.FullName
		if ($LASTEXITCODE -ne 0) { $code = 1 }
	}
	$global:LASTEXITCODE = $code
}
Step "validate_web" { node scripts/validate_web.mjs }
Step "input guard" { node scripts/test_input_guard.mjs }
Step "DOM check (fakultet.html)" { node web/tests/check-dom.js }

$build = Get-ChildItem out/build -Directory -ErrorAction SilentlyContinue | Where-Object { Test-Path (Join-Path $_.FullName "CTestTestfile.cmake") } | Select-Object -First 1
if ($build) {
	Step "ctest ($($build.Name))" { ctest --test-dir $build.FullName --output-on-failure }
} else {
	Write-Host "ctest preskočen (nema konfiguriranog nativnog builda)."
}

if ($failed -gt 0) { Write-Host "`n$failed provjera nije prošla." -ForegroundColor Red; exit 1 }
Write-Host "`nSve provjere prošle." -ForegroundColor Green
