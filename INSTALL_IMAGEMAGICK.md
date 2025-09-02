# 安装ImageMagick以支持图像转换

为了让solve-field.exe能够处理JPEG、PNG等图像格式，您需要安装ImageMagick。

## 方法1：下载并安装ImageMagick

1. 访问 https://imagemagick.org/script/download.php#windows
2. 下载适合您系统的版本（推荐64位版本）
3. 运行安装程序，确保选择"Install development headers and libraries for C and C++"
4. 安装完成后，将ImageMagick的bin目录添加到系统PATH环境变量中

## 方法2：使用包管理器安装

### 使用Chocolatey：
```cmd
choco install imagemagick
```

### 使用Scoop：
```cmd
scoop install imagemagick
```

## 验证安装

安装完成后，在命令行中运行以下命令验证：
```cmd
magick --version
```

应该显示ImageMagick的版本信息。

## 测试图像转换

安装ImageMagick后，您可以测试solve-field.exe：
```cmd
cd E:\github\astrometry.net_win\demo
solve-field apod1.jpg
```

## 替代方案

如果不想安装ImageMagick，您也可以：

1. **使用FITS格式的图像**：solve-field.exe原生支持FITS格式
2. **手动转换图像**：使用其他工具将JPEG转换为PPM格式
3. **安装Python astrometry模块**：安装完整的astrometry.net Python包

## 常见问题

**Q: 安装后仍然提示找不到magick命令？**
A: 确保ImageMagick的bin目录已添加到系统PATH环境变量中，并重启命令行窗口。

**Q: 可以使用其他图像转换工具吗？**
A: 可以，只要能将图像转换为PPM格式即可。您可以修改image2pnm.c来支持其他工具。
