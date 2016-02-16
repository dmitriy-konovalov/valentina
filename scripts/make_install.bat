rem script helps create installer

rem find target architecture
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set ARCHITECTURE=32BIT || set ARCHITECTURE=64BIT

rem Path to NSIS according to architecture
if %ARCHITECTURE%==32BIT set nsis_path="C:\Program Files\NSIS\makensisw.exe"
if %ARCHITECTURE%==64BIT set nsis_path="C:\Program Files (x86)\NSIS\makensisw.exe"

if not exist %nsis_path% (
	SET /P promt="Coudn't find NSIS. Do you want to continue?[Y\N]"
	IF "%promt%" == "Y" GOTO PREPARE
	IF "%promt%" == "y" GOTO PREPARE
	ELSE GOTO ONEXIT
) 

:PREPARE
cd ..
cd
rem force qmake create new qm files
del /Q share\translations\*.qm
mkdir build
cd build
cd

qmake -r ..\Valentina.pro
IF ERRORLEVEL 1 GOTO ERRORQMAKE1
IF ERRORLEVEL 0 GOTO MAKE

:MAKE
mingw32-make -j%NUMBER_OF_PROCESSORS%
IF ERRORLEVEL 1 GOTO ERRORMAKE
IF ERRORLEVEL 0 GOTO MAKEINSTALL

:MAKEINSTALL
mingw32-make install
IF ERRORLEVEL 1 GOTO ERRORMAKEINSTALL
IF ERRORLEVEL 0 GOTO ONEXIT

:ERRORMAKEINSTALL
echo Failed to create installer!
@pause
exit /b 1
:ERRORMAKE
echo Failed to build project!
@pause
exit /b 1
:ERRORQMAKE2
echo Failed to make the second run qmake!
@pause
exit /b 1
:ERRORQMAKE1
echo Failed to make the first run qmake!
@pause
exit /b 1 
:ONEXIT
echo Done!
@pause
