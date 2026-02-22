@echo off
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Enabling all Network Adapters...
    powershell -Command "Get-NetAdapter | Enable-NetAdapter -Confirm:$false"
    echo All network connections are now ON.
    timeout /t 3
) else (
    powershell -Command "Start-Process -FilePath '%0' -Verb RunAs"
)
