@echo off

set ProjectName=FactoryFarming

if not exist "win_build" mkdir "win_build"
cd win_build

set CodeDirectory=..\src
set OutputDirectory=..\Source
set PlaydateSdkDirectory=%PLAYDATE_SDK_PATH%
set PdcExeName=%PlaydateSdkDirectory%\bin\pdc
set OutputLibName=pdex.lib
set OutputDllName=pdex.dll
set OutputPdbName=pdex.pdb
set PdcOutputFolder=%ProjectName%.pdx

rem NOTE: Update this to find vcvarsall.bat on older VS installations, or if you installed VS 2022 somewhere non-default
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

rem NOTE: _USE_MATH_DEFINES is required for M_PI to be defined: https://stackoverflow.com/questions/26065359/m-pi-flagged-as-undeclared-identifier
set CompilerFlags=%CompilerFlags% /D "_WINDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "TARGET_SIMULATOR=1" /D "_WINDLL=1" /D "TARGET_EXTENSION=1" /D "_USE_MATH_DEFINES"
rem /GS = Buffer overrun protection is turned on
rem /Zi = Generates complete debugging information
rem /Gm- = Deprecated. Enables minimal rebuild
rem /Od = Disables optimization TODO: Debug only?
rem /RTC1 = Enable fast runtime checks (equivalent to /RTCsu)
rem /std:c++20 = C++20 standard ISO/IEC 14882:2020
rem /Gd = Uses the __cdecl calling convention (x86 only)
rem /MDd = Compiles to create a debug multithreaded DLL, by using MSVCRTD.lib
rem /Ob0 = Controls inline expansion (0 level = no expansion?)
rem /W3 = Set output warning level
rem /WX- = (Don't?) Treat warnings as errors TODO: Do we need this?
rem /nologo = Suppress the startup banner
set CompilerFlags=%CompilerFlags% /GS /Zi /Gm- /Od /RTC1 /std:c++20 /Gd /MDd /Ob0 /W3 /WX- /nologo
rem /Fp = Specifies a precompiled header file name TODO: Do we need this?
set CompilerFlags=%CompilerFlags% /Fp"%ProjectName%.pch" /Fd
rem /wd4098 = 'assignNeighbors': 'void' function returning a value
rem /wd4244 = 'return': conversion from 'double' to 'uint16_t', possible loss of data
rem /wd4996 = 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
rem /wd4267 = '=': conversion from 'size_t' to 'uint16_t', possible loss of data
rem /wd4018 = '>': signed/unsigned mismatch
rem /wd4101 = 'dummy': unreferenced local variable
set CompilerFlags=%CompilerFlags% /wd4098 /wd4244 /wd4996 /wd4267 /wd4018 /wd4101
rem /errorReport:prompt = Deprecated. Windows Error Reporting (WER) settings control error reporting TODO: Do we need this?
rem /diagnostics:column =  Diagnostics format: prints column information. TODO: Do we need this? (Optional)
rem /Zc:forScope = Enforce Standard C++ for scoping rules (on by default)
rem /Zc:inline = Remove unreferenced functions or data if they're COMDAT or have internal linkage only (off by default)
rem /Zc:wchar_t = wchar_t is a native type, not a typedef (on by default)
rem /fp:precise = "precise" floating-point model; results are predictable
set CompilerFlags=%CompilerFlags% /errorReport:prompt /Zc:forScope /Zc:inline /Zc:wchar_t /fp:precise
set CompilerFlags=%CompilerFlags% /I"%CodeDirectory%" /I"%PlaydateSdkDirectory%\C_API"
set LinkerFlags=/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG /DLL /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /TLBID:1
set LinkerFlags=%LinkerFlags% /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"%OutputDllName%.intermediate.manifest" /LTCGOUT:"%ProjectName%.iobj" /ILK:"%ProjectName%.ilk"
set Libraries="kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "comdlg32.lib" "advapi32.lib"
set PdcFlags=-q -sdkpath "%PlaydateSdkDirectory%"

set ObjectFiles=

for %%a in (%CodeDirectory%\*.c) do (
	CL %CompilerFlags% /c "%%a" /Fo"%%~na.obj"
	call set "ObjectFiles=%%ObjectFiles%% "%%~na.obj""
)
for %%a in (%CodeDirectory%\buildings\*.c) do (
	CL %CompilerFlags% /c "%%a" /Fo"%%~na.obj"
	call set "ObjectFiles=%%ObjectFiles%% "%%~na.obj""
)
for %%a in (%CodeDirectory%\ui\*.c) do (
	CL %CompilerFlags% /c "%%a" /Fo"%%~na.obj"
	call set "ObjectFiles=%%ObjectFiles%% "%%~na.obj""
)

LINK %LinkerFlags% %Libraries% %ObjectFiles% /OUT:"%OutputDllName%" /IMPLIB:"%OutputLibName%" /PDB:"%OutputPdbName%"

XCOPY ".\%OutputDllName%" "%OutputDirectory%\" /Y > NUL
%PdcExeName% %PdcFlags% "%OutputDirectory%" "%PdcOutputFolder%"
