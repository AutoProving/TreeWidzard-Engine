@echo off
:: SPDX-License-Identifier: MIT
:: Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

setlocal

set "SCRIPT_DIR=%~dp0"

call :resolve_build_dir
if errorlevel 1 (
    echo Error: treewidzard executable not found in any known build directory. 1>&2
    echo Set TREEWIDZARD_BUILD_DIR or build the project with: cmake -S . -B out\build\default ^&^& cmake --build out\build\default 1>&2
    exit /b 1
)

set "TREEWIDZARD_SEARCHSTRATEGIES=%BUILD_DIR%\SearchStrategies"
set "TREEWIDZARD_DPCORES=%BUILD_DIR%\DPCores"
set "PATH=%BUILD_DIR%;%PATH%"

"%BUILD_DIR%\treewidzard.exe" %*
exit /b %ERRORLEVEL%

:resolve_build_dir
if defined TREEWIDZARD_BUILD_DIR (
    if exist "%TREEWIDZARD_BUILD_DIR%\treewidzard.exe" (
        set "BUILD_DIR=%TREEWIDZARD_BUILD_DIR%"
        exit /b 0
    )
)

for %%D in ("%SCRIPT_DIR%out\build\default" "%SCRIPT_DIR%out\build\release" "%SCRIPT_DIR%out\build\debug" "%SCRIPT_DIR%out\build\ci") do (
    if exist "%%~fD\treewidzard.exe" (
        set "BUILD_DIR=%%~fD"
        exit /b 0
    )
)

for /d %%D in ("%SCRIPT_DIR%out\build\*") do (
    if exist "%%~fD\treewidzard.exe" (
        set "BUILD_DIR=%%~fD"
        exit /b 0
    )
)

for %%D in ("%SCRIPT_DIR%build" "%SCRIPT_DIR%Build" "%SCRIPT_DIR%build_port") do (
    if exist "%%~fD\treewidzard.exe" (
        set "BUILD_DIR=%%~fD"
        exit /b 0
    )
)

for /d %%D in ("%SCRIPT_DIR%build*") do (
    if exist "%%~fD\treewidzard.exe" (
        set "BUILD_DIR=%%~fD"
        exit /b 0
    )
)

exit /b 1
