@echo off
setlocal
cd /d "%~dp0"
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" exit /b 1
call "%VCVARS%"
if not exist dist mkdir dist
cl /nologo /LD /std:c++17 /utf-8 /EHsc /O2 /DUNICODE /D_UNICODE /I ..\include CodexQuotaTrafficMonitor.cpp /Fe:dist\CodexQuotaTrafficMonitor.dll user32.lib shell32.lib gdi32.lib
if errorlevel 1 exit /b 1
copy /Y runtime\*.ps1 dist\ >nul
copy /Y runtime\*.cjs dist\ >nul
copy /Y runtime\quota-state.example.json dist\quota-state.json >nul
