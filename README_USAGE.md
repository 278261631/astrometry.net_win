# Astrometry.net Windows版使用说明

## 🎉 编译完成！

恭喜！您已经成功在Windows上编译了astrometry.net工具套件。

## 📁 文件位置

所有可执行文件位于：`E:\github\astrometry.net_win\win\build\bin\`

### 主要工具：
- **solve-field.exe** - 主要的天体测量求解工具
- **astrometry-engine.exe** - 天体测量引擎
- **image2pnm.exe** - 图像格式转换工具
- **an-fitstopnm.exe** - FITS文件转换工具

## 🚀 快速开始

### 1. 处理FITS图像（直接支持）
```cmd
cd E:\github\astrometry.net_win\win\build\bin
solve-field.exe your_image.fits
```

### 2. 处理JPEG/PNG图像（需要ImageMagick）

**第一步：安装ImageMagick**
```cmd
# 运行我们提供的安装脚本
install-imagemagick.bat

# 或者手动安装Chocolatey然后安装ImageMagick
choco install imagemagick
```

**第二步：处理图像**
```cmd
cd E:\github\astrometry.net_win\win\build\bin
solve-field.exe your_image.jpg
```

## 🔧 当前状态

### ✅ 已完成：
- 所有核心C/C++代码编译成功
- Windows兼容性修复完成
- 可执行文件查找机制正常工作
- DLL依赖完全解决

### ⚠️ 需要额外安装：
- **ImageMagick**：用于处理JPEG、PNG、TIFF等图像格式
- **索引文件**：用于星空识别的星表数据

## 📋 错误解决

### 错误：`ModuleNotFoundError: No module named 'astrometry'`
**原因**：Python环境中缺少astrometry.net模块
**解决**：安装ImageMagick，image2pnm.exe会自动使用fallback模式

### 错误：`Image conversion failed`
**原因**：没有安装ImageMagick或其他图像转换工具
**解决**：
1. 运行 `install-imagemagick.bat`
2. 或手动安装ImageMagick
3. 或将图像转换为FITS格式

### 错误：`Couldn't find executable "xxx"`
**原因**：可执行文件路径问题
**解决**：确保在bin目录中运行，或将bin目录添加到PATH

## 🌟 下一步

1. **下载索引文件**：从 http://data.astrometry.net/ 下载适合您图像的索引文件
2. **配置索引路径**：使用 `--index-dir` 参数指定索引文件位置
3. **测试完整功能**：使用真实的天文图像进行测试

## 📞 技术支持

如果遇到问题，请检查：
1. 所有DLL文件是否在bin目录中
2. ImageMagick是否正确安装
3. 索引文件是否下载并配置正确

## 🎯 示例命令

```cmd
# 基本用法
solve-field.exe image.jpg

# 指定输出目录
solve-field.exe --dir output image.jpg

# 指定索引文件目录
solve-field.exe --index-dir C:\astrometry-indexes image.jpg

# 详细输出
solve-field.exe -v image.jpg

# 指定图像尺度范围
solve-field.exe --scale-low 0.1 --scale-high 10 --scale-units degwidth image.jpg
```

祝您使用愉快！🌟
