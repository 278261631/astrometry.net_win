# Astrometry.net Windows版本

欢迎使用astrometry.net的Windows移植版本！这是一个完整的天体测量解决方案，可以在Windows系统上识别和求解天文图像。

## 📚 文档指南

根据你的需求选择合适的文档：

### 🚀 新用户快速开始
- **[QUICK_START.md](QUICK_START.md)** - 5分钟快速设置指南
- **[INSTALLATION_CHECKLIST.txt](INSTALLATION_CHECKLIST.txt)** - 安装检查清单

### 📖 详细文档
- **[README_WINDOWS_SETUP.md](README_WINDOWS_SETUP.md)** - 完整的安装和使用指南

### 🔧 工具和脚本
- **[check_installation.bat](check_installation.bat)** - 自动检查安装状态
- **[build.bat](build.bat)** - 编译构建脚本

## ⚡ 快速安装摘要

### 必需下载
1. `netpbm-10.27-bin.zip`
2. `ImageMagick-7.1.2-3-Q16-x64-static.exe`
3. `gawk-3.1.6-1-bin.zip`
4. `jpeg-6b-4-bin.zip`

### 索引文件路径
```
E:/astrometry.net.index/
```

### 验证安装
```cmd
check_installation.bat
```

### 基本使用
```cmd
build\bin\solve-field.exe your_image.jpg
```

## 🎯 主要功能

- ✅ **图像格式支持**: JPEG, PNG, TIFF, FITS
- ✅ **自动源提取**: 使用simplexy算法
- ✅ **天体测量求解**: 自动识别星座和坐标
- ✅ **多种输出格式**: WCS, 匹配文件, 源列表
- ✅ **可配置参数**: 比例范围, CPU限制, 详细输出

## 🏆 技术成就

这个Windows移植版本解决了以下技术挑战：
- 跨平台内存映射兼容性
- Windows文件权限和路径处理
- Python模块系统重构
- 复杂的构建系统移植
- 图像处理工具链集成

## 📊 系统要求

- **操作系统**: Windows 10/11 (64位)
- **内存**: 最少4GB，推荐8GB+
- **磁盘空间**: 至少10GB（用于索引文件）
- **处理器**: 现代多核CPU推荐

## 🔍 故障排除

如果遇到问题：

1. **运行安装检查**:
   ```cmd
   check_installation.bat
   ```

2. **查看详细错误**:
   ```cmd
   build\bin\solve-field.exe --verbose your_image.jpg
   ```

3. **检查常见问题**:
   - PATH环境变量是否正确设置
   - 索引文件是否存在于正确路径
   - 所有依赖软件是否已安装

## 📈 性能优化建议

- 只下载你需要的索引文件规模
- 使用`--cpulimit`参数避免长时间运行
- 对大图像进行适当缩放
- 确保有足够的可用内存

## 🤝 贡献和支持

这是astrometry.net的社区Windows移植版本。如果你发现问题或有改进建议，欢迎反馈。

## 📄 许可证

遵循原始astrometry.net项目的许可证条款。

---

**开始使用**: 阅读 [QUICK_START.md](QUICK_START.md) 立即开始！
