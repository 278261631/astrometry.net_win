@echo off
echo ===============================================================================
echo                    ASTROMETRY.NET WINDOWS - Installation Check
echo ===============================================================================
echo.

echo [1] Checking basic tools...
echo.

echo Checking jpegtopnm (NetPBM):
jpegtopnm --version >nul 2>&1
if %errorlevel%==0 (
    echo   [OK] jpegtopnm available
) else (
    echo   [X] jpegtopnm not found
)

echo Checking gawk:
gawk --version >nul 2>&1
if %errorlevel%==0 (
    echo   [OK] gawk available
) else (
    echo   [X] gawk not found
)

echo.
echo [2] Checking index directory...
if exist "E:\astrometry.net.index\" (
    echo   [OK] Index directory exists: E:\astrometry.net.index\
    
    dir /b "E:\astrometry.net.index\*.fits" >nul 2>&1
    if %errorlevel%==0 (
        echo   [OK] FITS index files found
    ) else (
        echo   [X] No FITS index files found
    )
) else (
    echo   [X] Index directory missing: E:\astrometry.net.index\
)

echo.
echo [3] Checking compiled executables...
if exist "build\bin\solve-field.exe" (
    echo   [OK] solve-field.exe exists
) else (
    echo   [X] solve-field.exe missing
)

if exist "build\bin\astrometry-engine.exe" (
    echo   [OK] astrometry-engine.exe exists
) else (
    echo   [X] astrometry-engine.exe missing
)

echo.
echo [4] Checking copied tools...
if exist "build\bin\jpegtopnm.exe" (
    echo   [OK] jpegtopnm.exe copied to build/bin
) else (
    echo   [!] jpegtopnm.exe not copied - run copy_tools.bat
)

if exist "build\bin\gawk.exe" (
    echo   [OK] gawk.exe copied to build/bin
) else (
    echo   [!] gawk.exe not copied - run copy_tools.bat
)

if exist "build\bin\jpeg62.dll" (
    echo   [OK] jpeg62.dll copied to build/bin
) else (
    echo   [!] jpeg62.dll not copied - may need manual copy
)

if exist "build\bin\libnetpbm10.dll" (
    echo   [OK] libnetpbm10.dll copied to build/bin
) else (
    echo   [!] libnetpbm10.dll not copied - may need manual copy
)

echo.
echo [5] Testing solve-field...
if exist "build\bin\solve-field.exe" (
    build\bin\solve-field.exe --help >nul 2>&1
    if %errorlevel%==0 (
        echo   [OK] solve-field.exe runs correctly
    ) else (
        echo   [X] solve-field.exe failed to run
    )
) else (
    echo   [X] solve-field.exe not found
)

echo.
echo ===============================================================================
echo                                Check Complete
echo ===============================================================================
echo.

echo Legend:
echo   [OK] - Working correctly
echo   [X]  - Problem found, needs attention
echo   [!]  - Warning, may need manual action
echo.

echo Quick test command:
echo   build\bin\solve-field.exe --help
echo.

pause
