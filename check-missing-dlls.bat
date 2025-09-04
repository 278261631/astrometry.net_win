@echo off
echo Checking for missing DLLs...
echo.

cd /d "E:\github\astrometry.net_win\win\build\bin"

echo Current DLLs in bin directory:
dir *.dll /b
echo.

echo Attempting to run solve-field.exe to identify missing DLLs...
echo This will show error dialogs for each missing DLL.
echo.

echo Press any key to test solve-field.exe (this will show missing DLL errors)...
pause

solve-field.exe --help 2>error.log

echo.
echo If there were missing DLL errors, they should have appeared as dialog boxes.
echo.

echo Common missing DLLs that might be needed:
echo - libssl-3-x64.dll
echo - libcrypto-3-x64.dll  
echo - libnghttp2-14.dll
echo - libpsl-5.dll
echo - libunistring-5.dll
echo - libzstd.dll
echo - libssh2-1.dll
echo.

echo You can copy these from C:\msys64\mingw64\bin\ if they exist there.
echo.
pause
