@echo off
:: Check for Admin rights and request them if not present
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Starting your CLI Tool...
    netsh interface set interface "Wi-Fi" disable
    echo Wi-Fi is now OFF.
    timeout /t 2
) else (
    echo Starting the CLI Tool...
    powershell -Command "Start-Process -FilePath '%0' -Verb RunAs"
)
