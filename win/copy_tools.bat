@echo off
echo ===============================================================================
echo                    复制NetPBM和GAWK工具到build/bin目录
echo ===============================================================================
echo.

set SOURCE_DIR=C:\GnuWin32\bin
set DEST_DIR=build\bin

echo 源目录: %SOURCE_DIR%
echo 目标目录: %DEST_DIR%
echo.

if not exist "%DEST_DIR%" (
    echo 创建目标目录: %DEST_DIR%
    mkdir "%DEST_DIR%"
)

echo 正在复制NetPBM工具...
echo.

REM NetPBM图像转换工具
call :copy_if_exists "%SOURCE_DIR%\bmptopnm.exe" "%DEST_DIR%\bmptopnm.exe" "bmptopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\jpegtopnm.exe" "%DEST_DIR%\jpegtopnm.exe" "jpegtopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmtopgm.exe" "%DEST_DIR%\pnmtopgm.exe" "pnmtopgm.exe"
call :copy_if_exists "%SOURCE_DIR%\ppmtopgm.exe" "%DEST_DIR%\ppmtopgm.exe" "ppmtopgm.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmfile.exe" "%DEST_DIR%\pnmfile.exe" "pnmfile.exe"
call :copy_if_exists "%SOURCE_DIR%\pngtopnm.exe" "%DEST_DIR%\pngtopnm.exe" "pngtopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\tifftopnm.exe" "%DEST_DIR%\tifftopnm.exe" "tifftopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\giftopnm.exe" "%DEST_DIR%\giftopnm.exe" "giftopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\fitstopnm.exe" "%DEST_DIR%\fitstopnm.exe" "fitstopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmtofits.exe" "%DEST_DIR%\pnmtofits.exe" "pnmtofits.exe"
call :copy_if_exists "%SOURCE_DIR%\pamtopnm.exe" "%DEST_DIR%\pamtopnm.exe" "pamtopnm.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmtopam.exe" "%DEST_DIR%\pnmtopam.exe" "pnmtopam.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmscale.exe" "%DEST_DIR%\pnmscale.exe" "pnmscale.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmflip.exe" "%DEST_DIR%\pnmflip.exe" "pnmflip.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmrotate.exe" "%DEST_DIR%\pnmrotate.exe" "pnmrotate.exe"
call :copy_if_exists "%SOURCE_DIR%\pnmcrop.exe" "%DEST_DIR%\pnmcrop.exe" "pnmcrop.exe"

echo.
echo 正在复制GAWK工具...
echo.

REM GAWK文本处理工具
call :copy_if_exists "%SOURCE_DIR%\gawk.exe" "%DEST_DIR%\gawk.exe" "gawk.exe"
call :copy_if_exists "%SOURCE_DIR%\awk.exe" "%DEST_DIR%\awk.exe" "awk.exe"

echo.
echo 正在复制相关的DLL文件...
echo.

REM 复制可能需要的DLL文件
call :copy_if_exists "%SOURCE_DIR%\jpeg62.dll" "%DEST_DIR%\jpeg62.dll" "jpeg62.dll"
call :copy_if_exists "%SOURCE_DIR%\libpng13.dll" "%DEST_DIR%\libpng13.dll" "libpng13.dll"
call :copy_if_exists "%SOURCE_DIR%\libtiff3.dll" "%DEST_DIR%\libtiff3.dll" "libtiff3.dll"
call :copy_if_exists "%SOURCE_DIR%\zlib1.dll" "%DEST_DIR%\zlib1.dll" "zlib1.dll"

echo.
echo ===============================================================================
echo                                复制完成
echo ===============================================================================
echo.

echo 验证复制的工具:
if exist "%DEST_DIR%\jpegtopnm.exe" (
    echo   ✓ jpegtopnm.exe
) else (
    echo   ✗ jpegtopnm.exe 未找到
)

if exist "%DEST_DIR%\ppmtopgm.exe" (
    echo   ✓ ppmtopgm.exe
) else (
    echo   ✗ ppmtopgm.exe 未找到
)

if exist "%DEST_DIR%\gawk.exe" (
    echo   ✓ gawk.exe
) else (
    echo   ✗ gawk.exe 未找到
)

echo.
echo 如果有工具未找到，请确保已正确安装NetPBM和GAWK到 C:\GnuWin32\
echo 或者手动将工具复制到 %DEST_DIR% 目录中。
echo.

pause
goto :eof

REM 函数：如果文件存在则复制
:copy_if_exists
set source_file=%1
set dest_file=%2
set tool_name=%3

if exist %source_file% (
    copy /Y %source_file% %dest_file% >nul 2>&1
    if %errorlevel%==0 (
        echo   ✓ 已复制 %tool_name%
    ) else (
        echo   ✗ 复制失败 %tool_name%
    )
) else (
    echo   - %tool_name% 未找到，跳过
)
goto :eof
