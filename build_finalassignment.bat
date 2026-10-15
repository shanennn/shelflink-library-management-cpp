@echo off
setlocal

set "ROOT=%~dp0"
set "SOURCE_DIR=%ROOT%FinalAssignment\FinalAssignment"
set "OUTPUT_DIR=%ROOT%FinalAssignment\x64\Debug"
set "EXE_FILE=%OUTPUT_DIR%\FinalAssignment.exe"
set "OBJ_DIR=%TEMP%\FinalAssignment_obj_%RANDOM%%RANDOM%"

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

if not exist "%OBJ_DIR%" (
    mkdir "%OBJ_DIR%"
)

set "VSINSTALL="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VSINSTALL=%%i"
    )
)

if not defined VSINSTALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvars64.bat" set "VSINSTALL=C:\Program Files\Microsoft Visual Studio\2022\Preview"
if not defined VSINSTALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" set "VSINSTALL=C:\Program Files\Microsoft Visual Studio\2022\Community"
if not defined VSINSTALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" set "VSINSTALL=C:\Program Files\Microsoft Visual Studio\2022\Professional"
if not defined VSINSTALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" set "VSINSTALL=C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
if not defined VSINSTALL if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" set "VSINSTALL=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"

if not defined VSINSTALL (
    echo Visual Studio C++ Build Tools not found.
    echo Please install Desktop development with C++.
    exit /b 1
)

call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1

pushd "%SOURCE_DIR%"

cl /nologo /EHsc /std:c++20 /Fo:"%OBJ_DIR%\\" /Fe:"%EXE_FILE%" ^
    "Globals.cpp" ^
    "main.cpp" ^
    "Module1.cpp" ^
    "Module2.cpp" ^
    "Module3.cpp" ^
    "Module4.cpp"

set "BUILD_RESULT=%ERRORLEVEL%"
popd

if not "%BUILD_RESULT%"=="0" (
    echo.
    echo Build failed.
    echo If FinalAssignment.exe is still running, close it and build again.
)

exit /b %BUILD_RESULT%
