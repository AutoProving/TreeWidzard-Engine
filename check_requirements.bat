@echo off
:: SPDX-License-Identifier: MIT
:: Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

setlocal EnableExtensions EnableDelayedExpansion

set "FAILURES=0"

echo TreeWidzard source-build requirement check
echo Platform: Windows
echo.

if not defined MSYSTEM (
    echo [NOTE] Windows source builds are expected to run from the MSYS2 MinGW 64-bit environment.
    echo.
)

call :check_cxx20
call :check_cmake
call :check_ninja
call :check_optional_bison
call :check_optional_flex

echo.
if "%FAILURES%"=="0" (
    echo Result: this system meets the default Windows source-build requirements.
    echo.
    echo Next steps:
    echo   mkdir build
    echo   cd build
    echo   cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    echo   ninja
    exit /b 0
)

echo Result: this system is missing one or more required Windows source-build tools.
echo Suggested install command in the MSYS2 MinGW 64-bit terminal:
echo   pacman -S --needed git mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
echo.
echo Then rerun:
echo   check_requirements.bat
exit /b 1

:check_cxx20
where g++ >nul 2>nul
if errorlevel 1 (
    call :fail "g++ not found"
    goto :eof
)

call :pass "Using C++ compiler: g++"
set "TMP_CPP=%TEMP%\treewidzard_check_%RANDOM%.cpp"
set "TMP_OBJ=%TEMP%\treewidzard_check_%RANDOM%.obj"
(
    echo #include ^<vector^>
    echo int main() { std::vector<int^> v{1, 2, 3}; return (int)v.size() - 3; }
) > "%TMP_CPP%"

g++ -std=c++20 -c "%TMP_CPP%" -o "%TMP_OBJ%" >nul 2>nul
if errorlevel 1 (
    del /q "%TMP_CPP%" "%TMP_OBJ%" >nul 2>nul
    call :fail "g++ does not compile a simple C++20 source file"
    goto :eof
)

del /q "%TMP_CPP%" "%TMP_OBJ%" >nul 2>nul
call :pass "g++ accepts -std=c++20"
goto :eof

:check_cmake
where cmake >nul 2>nul
if errorlevel 1 (
    call :fail "CMake not found"
    goto :eof
)

for /f "tokens=3" %%V in ('cmake --version ^| findstr /B /C:"cmake version"') do set "CMAKE_VERSION=%%V"
if not defined CMAKE_VERSION (
    call :fail "Could not determine CMake version"
    goto :eof
)

call :check_version "CMake" "!CMAKE_VERSION!" "3.18.0"
goto :eof

:check_optional_bison
set "BISON_VERSION="
where bison >nul 2>nul
if errorlevel 1 (
    call :optional "Bison not found (only needed when regenerating the parser)"
    goto :eof
)

for /f "tokens=4" %%V in ('bison --version ^| more') do (
    set "BISON_VERSION=%%V"
    goto check_bison_version
)

:check_bison_version
if not defined BISON_VERSION (
    call :optional "Could not determine Bison version (only needed when regenerating the parser)"
    goto :eof
)

call :check_optional_version "Bison" "!BISON_VERSION!" "3.0.0"
goto :eof

:check_optional_flex
set "FLEX_VERSION="
where flex >nul 2>nul
if errorlevel 1 (
    call :optional "Flex not found (only needed when regenerating the parser)"
    goto :eof
)

for /f "tokens=2" %%V in ('flex --version') do set "FLEX_VERSION=%%V"
if not defined FLEX_VERSION (
    call :optional "Could not determine Flex version (only needed when regenerating the parser)"
    goto :eof
)

call :check_optional_version "Flex" "!FLEX_VERSION!" "2.6.0"
goto :eof

:check_ninja
where ninja >nul 2>nul
if errorlevel 1 (
    call :fail "Ninja not found"
    goto :eof
)

call :pass "Ninja found"
goto :eof

:check_version
set "LABEL=%~1"
set "FOUND_VERSION=%~2"
set "MIN_VERSION=%~3"

powershell -NoProfile -Command "if ([version]'!FOUND_VERSION!' -ge [version]'!MIN_VERSION!') { exit 0 } else { exit 1 }" >nul 2>nul
if errorlevel 1 (
    call :fail "!LABEL! version !FOUND_VERSION! is below minimum !MIN_VERSION!"
    goto :eof
)

call :pass "!LABEL! version !FOUND_VERSION! satisfies minimum !MIN_VERSION!"
goto :eof

:check_optional_version
set "LABEL=%~1"
set "FOUND_VERSION=%~2"
set "MIN_VERSION=%~3"

powershell -NoProfile -Command "if ([version]'!FOUND_VERSION!' -ge [version]'!MIN_VERSION!') { exit 0 } else { exit 1 }" >nul 2>nul
if errorlevel 1 (
    call :optional "!LABEL! version !FOUND_VERSION! is below recommended !MIN_VERSION! for parser regeneration"
    goto :eof
)

call :optional "!LABEL! version !FOUND_VERSION! is available for parser regeneration"
goto :eof

:pass
echo [OK] %~1
goto :eof

:optional
echo [OPTIONAL] %~1
goto :eof

:fail
echo [MISSING] %~1
set /a FAILURES+=1
goto :eof
