@echo off
echo ===============================================================================
echo                    ASTROMETRY.NET WINDOWS版本 - 安装检查工具
echo ===============================================================================
echo.

echo [1/7] 检查基本工具...
echo.

echo 检查 jpegtopnm (NetPBM):
jpegtopnm --version >nul 2>&1
if %errorlevel%==0 (
    echo   [OK] jpegtopnm 可用
) else (
    echo   [X] jpegtopnm 未找到 - 请安装NetPBM并添加到PATH
)

echo 检查 ppmtopgm (NetPBM):
ppmtopgm --help >nul 2>&1
if %errorlevel%==0 (
    echo   ✓ ppmtopgm 可用
) else (
    echo   ✗ ppmtopgm 未找到 - 请安装NetPBM并添加到PATH
)

echo 检查 gawk:
gawk --version >nul 2>&1
if %errorlevel%==0 (
    echo   ✓ gawk 可用
) else (
    echo   ✗ gawk 未找到 - 请安装GAWK并添加到PATH
)

echo 检查 ImageMagick:
magick --version >nul 2>&1
if %errorlevel%==0 (
    echo   ✓ ImageMagick 可用
) else (
    echo   ✗ ImageMagick 未找到 - 请安装ImageMagick
)

echo.
echo [2/7] 检查索引文件目录...
if exist "E:\astrometry.net.index\" (
    echo   ✓ 索引目录存在: E:\astrometry.net.index\
    
    echo   检查索引文件:
    dir /b "E:\astrometry.net.index\*.fits" >nul 2>&1
    if %errorlevel%==0 (
        echo   ✓ 找到FITS索引文件
        for /f %%i in ('dir /b "E:\astrometry.net.index\*.fits" 2^>nul ^| find /c /v ""') do set count=%%i
        echo     文件数量: !count!
    ) else (
        echo   ✗ 未找到FITS索引文件
        echo     请从 http://data.astrometry.net/ 下载索引文件
    )
) else (
    echo   ✗ 索引目录不存在: E:\astrometry.net.index\
    echo     请创建此目录并下载索引文件
)

echo.
echo [3/7] 检查编译输出...
if exist "build\bin\solve-field.exe" (
    echo   ✓ solve-field.exe 存在
) else (
    echo   ✗ solve-field.exe 不存在 - 请运行 build.bat 编译
)

if exist "build\bin\astrometry-engine.exe" (
    echo   ✓ astrometry-engine.exe 存在
) else (
    echo   ✗ astrometry-engine.exe 不存在 - 请运行 build.bat 编译
)

if exist "build\bin\simplexy.exe" (
    echo   ✓ simplexy.exe 存在
) else (
    echo   ✗ simplexy.exe 不存在 - 请运行 build.bat 编译
)

echo.
echo [3.5/7] 检查复制的工具...
if exist "build\bin\jpegtopnm.exe" (
    echo   ✓ jpegtopnm.exe 已复制到build/bin
) else (
    echo   ! jpegtopnm.exe 未复制 - 可运行 copy_tools.bat 手动复制
)

if exist "build\bin\ppmtopgm.exe" (
    echo   ✓ ppmtopgm.exe 已复制到build/bin
) else (
    echo   ! ppmtopgm.exe 未复制 - 可运行 copy_tools.bat 手动复制
)

if exist "build\bin\gawk.exe" (
    echo   ✓ gawk.exe 已复制到build/bin
) else (
    echo   ! gawk.exe 未复制 - 可运行 copy_tools.bat 手动复制
)

echo.
echo [4/7] 检查配置文件...
if exist "..\etc\astrometry.cfg" (
    echo   ✓ 配置文件存在: ..\etc\astrometry.cfg
) else (
    echo   ✗ 配置文件不存在: ..\etc\astrometry.cfg
)

echo.
echo [5/7] 测试solve-field帮助信息...
if exist "build\bin\solve-field.exe" (
    build\bin\solve-field.exe --help >nul 2>&1
    if %errorlevel%==0 (
        echo   ✓ solve-field.exe 可以运行
    ) else (
        echo   ✗ solve-field.exe 运行失败
    )
) else (
    echo   ✗ solve-field.exe 不存在，跳过测试
)

echo.
echo [6/7] 检查临时目录...
if exist "temp\" (
    echo   ✓ 临时目录存在: temp\
) else (
    echo   ! 临时目录不存在，将在运行时自动创建
)

echo.
echo [7/7] 检查演示文件...
if exist "..\demo\apod1.jpg" (
    echo   ✓ 演示图像存在: ..\demo\apod1.jpg
) else (
    echo   ! 演示图像不存在: ..\demo\apod1.jpg
)

echo.
echo ===============================================================================
echo                                检查完成
echo ===============================================================================
echo.

echo 如果所有项目都显示 ✓，则安装正确。
echo 如果有 ✗ 项目，请参考 README_WINDOWS_SETUP.md 解决问题。
echo.

echo 快速测试命令:
echo   build\bin\solve-field.exe --help
echo   build\bin\solve-field.exe --verbose ..\demo\apod1.jpg
echo.

pause
