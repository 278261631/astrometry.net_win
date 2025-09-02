# Astrometry.net Windows版本安装指南

## 概述

这是astrometry.net的Windows移植版本，支持在Windows系统上进行天体测量和图像求解。

## 系统要求

- Windows 10/11 (64位)
- 至少4GB内存
- 至少10GB可用磁盘空间（用于索引文件）

## 必需的依赖软件

在使用astrometry.net之前，请下载并安装以下软件包：

### 1. NetPBM工具包
**文件**: `netpbm-10.27-bin.zip`
**用途**: 图像格式转换（JPEG → PPM → PGM）
**安装**:
1. 下载netpbm-10.27-bin.zip
2. 解压到 `C:\GnuWin32\`
3. 确保 `C:\GnuWin32\bin` 在系统PATH中



### 3. GAWK
**文件**: `gawk-3.1.6-1-bin.zip`
**用途**: 文本处理工具
**安装**:
1. 下载gawk-3.1.6-1-bin.zip
2. 解压到 `C:\GnuWin32\`
3. 确保 `C:\GnuWin32\bin` 在系统PATH中

### 4. JPEG库
**文件**: `jpeg-6b-4-bin.zip`
**用途**: JPEG图像处理
**安装**:
1. 下载jpeg-6b-4-bin.zip
2. 解压到 `C:\GnuWin32\`
3. 确保 `C:\GnuWin32\bin` 在系统PATH中

## 索引文件配置

### 索引文件路径
**必需路径**: `E:/astrometry.net.index/`

### 下载索引文件
1. 创建目录 `E:\astrometry.net.index\`
2. 从 http://data.astrometry.net/ 下载所需的索引文件
3. 将索引文件放置在 `E:\astrometry.net.index\` 目录中

### 推荐的索引文件
根据你的图像规模选择合适的索引文件：
- **大视场 (>30度)**: index-4219.fits, index-4218.fits
- **中等视场 (1-30度)**: index-4217.fits 到 index-4210.fits
- **小视场 (<1度)**: index-4209.fits 到 index-4207-xx.fits

## 编译和构建

### 前提条件
- CMake 3.15+
- MinGW-w64或Visual Studio 2019+
- Git

### 构建步骤
1. 打开命令提示符
2. 进入win目录：
   ```cmd
   cd E:\github\astrometry.net_win\win
   ```
3. 运行构建脚本：
   ```cmd
   build.bat
   ```

### 工具复制
构建过程会自动尝试复制NetPBM和GAWK工具到build/bin目录。如果自动复制失败，可以手动运行：
```cmd
copy_tools.bat
```

这将复制以下工具（如果存在）：
- **NetPBM工具**: jpegtopnm.exe, ppmtopgm.exe, pnmfile.exe等
- **GAWK工具**: gawk.exe, awk.exe
- **相关DLL**: jpeg62.dll, libpng13.dll等

## 使用方法

### 基本用法
```cmd
build\bin\solve-field.exe your_image.jpg
```

### 常用参数
```cmd
# 指定图像比例范围
build\bin\solve-field.exe --scale-low 0.5 --scale-high 2.0 image.jpg

# 设置CPU时间限制
build\bin\solve-field.exe --cpulimit 60 image.jpg

# 覆盖现有输出文件
build\bin\solve-field.exe --overwrite image.jpg

# 详细输出
build\bin\solve-field.exe --verbose image.jpg
```

## 输出文件

成功求解后，将生成以下文件：
- `image.axy` - 源列表文件
- `image.wcs` - 世界坐标系统文件
- `image.solved` - 求解状态文件
- `image.match` - 匹配结果文件

## 故障排除

### 常见问题

1. **"找不到可执行文件"错误**
   - 检查PATH环境变量是否包含所有必需的工具路径
   - 确保所有依赖软件都已正确安装

2. **"无法加载索引文件"错误**
   - 确认索引文件路径为 `E:\astrometry.net.index\`
   - 检查索引文件是否完整下载

3. **"内存映射失败"错误**
   - 确保有足够的可用内存
   - 尝试使用较小的索引文件

4. **"图像转换失败"错误**
   - 确认NetPBM工具已正确安装
   - 检查输入图像格式是否支持

### 调试模式
使用 `--verbose` 参数获取详细的调试信息：
```cmd
build\bin\solve-field.exe --verbose --no-delete-temp image.jpg
```

## 性能优化

### 提高求解速度
1. 使用适当规模的索引文件
2. 设置合理的比例范围参数
3. 限制CPU时间避免长时间运行

### 内存使用优化
1. 关闭不必要的应用程序
2. 使用较小的图像尺寸
3. 选择合适的索引文件数量

## 技术支持

如果遇到问题，请检查：
1. 所有依赖软件是否正确安装
2. 环境变量PATH是否正确配置
3. 索引文件是否完整
4. 输入图像格式是否支持

## 版本信息

- 基于astrometry.net原版移植
- Windows移植版本：1.0
- 支持的图像格式：JPEG, PNG, TIFF, FITS
- 编译环境：MinGW-w64, CMake

---

**注意**: 这是astrometry.net的Windows移植版本，某些功能可能与原版略有不同。建议在使用前先用测试图像验证功能是否正常。
