# Astrometry.net Windows版本 - 快速开始指南

## 🚀 5分钟快速设置

### 第1步：下载依赖软件
下载以下4个文件：
- `netpbm-10.27-bin.zip`
- `ImageMagick-7.1.2-3-Q16-x64-static.exe`
- `gawk-3.1.6-1-bin.zip`
- `jpeg-6b-4-bin.zip`

### 第2步：安装
1. **解压3个zip文件到 `C:\GnuWin32\`**
   ```
   netpbm-10.27-bin.zip → C:\GnuWin32\
   gawk-3.1.6-1-bin.zip → C:\GnuWin32\
   jpeg-6b-4-bin.zip → C:\GnuWin32\
   ```

2. **运行ImageMagick安装程序**
   - 双击 `ImageMagick-7.1.2-3-Q16-x64-static.exe`
   - 使用默认设置安装

3. **添加到PATH环境变量**
   - 右键"此电脑" → 属性 → 高级系统设置 → 环境变量
   - 在系统变量中找到"Path"，点击编辑
   - 添加：`C:\GnuWin32\bin`

### 第3步：创建索引目录
创建文件夹：`E:\astrometry.net.index\`

### 第4步：下载索引文件
从 http://data.astrometry.net/ 下载索引文件到 `E:\astrometry.net.index\`

**推荐下载**（根据你的图像选择）：
- 大视场图像：`index-4217.fits`, `index-4216.fits`
- 中等视场：`index-4215.fits`, `index-4214.fits`, `index-4213.fits`
- 小视场：`index-4212.fits`, `index-4211.fits`

### 第5步：编译
```cmd
cd E:\github\astrometry.net_win\win
build.bat
```

**注意**: 构建过程会自动复制NetPBM和GAWK工具到build/bin目录。如果需要手动复制，运行：
```cmd
copy_tools.bat
```

## 🎯 立即测试

### 基本用法
```cmd
build\bin\solve-field.exe your_image.jpg
```

### 推荐的第一次测试
```cmd
build\bin\solve-field.exe --verbose --cpulimit 60 --overwrite test_image.jpg
```

## 📁 输出文件说明

成功后会生成：
- `image.axy` - 检测到的星点
- `image.wcs` - 坐标系统信息
- `image.solved` - 求解成功标记

## ⚡ 常用参数

```cmd
# 指定图像比例（角秒/像素）
--scale-low 0.5 --scale-high 2.0

# 限制运行时间（秒）
--cpulimit 60

# 覆盖已存在的文件
--overwrite

# 详细输出（调试用）
--verbose

# 不删除临时文件（调试用）
--no-delete-temp
```

## 🔧 快速故障排除

### 问题：找不到jpegtopnm
**解决**：确保netpbm已解压到C:\GnuWin32\且PATH已设置

### 问题：找不到索引文件
**解决**：确认索引文件在E:\astrometry.net.index\目录

### 问题：程序崩溃
**解决**：使用--verbose参数查看详细错误信息

## 📊 性能建议

- **内存**: 至少4GB，推荐8GB+
- **索引文件**: 只下载你需要的比例范围
- **图像大小**: 建议不超过4000x4000像素
- **CPU限制**: 设置--cpulimit避免长时间运行

## 🎉 成功标志

如果看到以下输出，说明安装成功：
```
Reading input file 1 of 1: "your_image.jpg"...
Extracting sources...
simplexy: found XXXX sources.
Solving...
Field: your_image.jpg
Field solved!
```

---

**需要帮助？** 查看完整文档：`README_WINDOWS_SETUP.md`
