@echo off
cd %~dp0
set NODEJS_DIR=d:\dev_apps\nodejs
set SRC_DIR=%cd%\src
set BUILD_DIR=%cd%\build

call %NODEJS_DIR%\nodevars.bat

if exist %BUILD_DIR% del /s /q %BUILD_DIR%
mkdir %BUILD_DIR%

pushd %SRC_DIR%
call tsc --outDir %BUILD_DIR% key.ts objects.ts shapes.ts spacedebris.ts 
popd
copy /y index.html %BUILD_DIR%
xcopy /s /i resources %BUILD_DIR%\resources
