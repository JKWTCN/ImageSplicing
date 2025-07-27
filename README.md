# ImageSplicing - 图像拼接工具

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Qt](https://img.shields.io/badge/Qt-5.x-green.svg)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-orange.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)

一个功能强大的图像拼接工具，支持水平和垂直方向的智能图像拼接，提供直观的图形化界面和丰富的配置选项。

## 🌟 主要特性

- **多种拼接模式**
  - 水平拼接：将多张图片左右排列
  - 垂直拼接：将多张图片上下排列
  - 自动调整：智能优化图像对齐

- **智能图像处理**
  - 自动尺寸调整（高到低、低到高、保持原始）
  - 多种缩放算法支持
  - 图像质量优化

- **便捷的操作体验**
  - 拖拽文件直接导入
  - 图像顺序调整（上移、下移、删除、复制）
  - 实时预览效果
  - 可视化拼接线编辑

- **丰富的配置选项**
  - 多种输出格式（PNG、JPG）
  - 边缘填充颜色自定义
  - 图像处理参数调节
  - 保存路径记忆

- **交互式编辑**
  - 可移动的图像元素
  - 层级管理（图层上移/下移）
  - 缩放预览（Ctrl+滚轮）
  - 选择性编辑功能

## 📋 系统要求

### 依赖项

- **Qt 5.x** - 图形界面框架
- **OpenCV 4.x** - 图像处理库
- **CMake 3.10+** - 构建系统
- **C++11** 兼容编译器

### 支持的操作系统

- Windows 10/11
- 支持其他Qt和OpenCV兼容的平台

### 支持的图像格式

- JPG/JPEG
- PNG
- BMP
- GIF
- TIF/TIFF
- ICO

## 🚀 安装与构建

### 1. 克隆项目

```bash
git clone https://github.com/JKWTCN/ImageSplicing.git
cd ImageSplicing
```

### 2. 安装依赖

#### Windows

1. 安装 [Qt 5.x](https://www.qt.io/download)
2. 安装 [OpenCV](https://opencv.org/releases/) 并配置环境变量
3. 安装 [CMake](https://cmake.org/download/)

### 3. 构建项目

#### 使用 CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

#### 使用 Visual Studio Code

1. 打开项目文件夹
2. 使用 `Ctrl+Shift+P` 打开命令面板
3. 运行 `CMake: Configure`
4. 运行 `CMake: Build`

或者使用已配置的构建任务：

```bash
# 在 VS Code 中按 Ctrl+Shift+P
# 选择 "Tasks: Run Task"
# 选择 "Build ImageSplicing Project"
```

## 📖 使用指南

### 基本操作流程

1. **导入图像**
   - 点击"打开文件"按钮选择多张图片
   - 或直接拖拽图片文件到程序窗口

2. **调整图像顺序**
   - 在左侧列表中选择图像
   - 使用上移/下移按钮调整顺序
   - 使用复制/删除按钮管理图像

3. **选择拼接模式**
   - 点击"水平拼接"进行左右排列
   - 点击"垂直拼接"进行上下排列
   - 使用"自动调整"优化对齐效果

4. **调整和预览**
   - 在右侧预览区域查看效果
   - 使用 Ctrl+滚轮缩放预览
   - 拖拽图像元素进行微调

5. **保存结果**
   - 点击"保存"按钮导出拼接后的图像
   - 选择输出格式和保存路径

### 高级功能

#### 拼接线编辑

- 拼接线显示图像之间的边界
- 可以通过拖拽调整图像位置
- 支持精确的像素级别调整

#### 层级管理

- 选择图像后使用"图层上移/下移"调整层级
- 控制图像的显示顺序

#### 配置选项

打开"设置"菜单进行个性化配置：

- **图像处理模式**：高到低、低到高、保持原始
- **缩放算法**：选择适合的图像缩放方法
- **输出格式**：PNG（支持透明）或 JPG
- **边缘填充颜色**：自定义背景颜色
- **其他选项**：文件导入顺序、完成后处理等

## 🎯 主要功能模块

### 核心组件

| 文件 | 描述 |
|------|------|
| `mainwindow.cpp/h` | 主窗口逻辑，核心拼接功能 |
| `MovablePixmapItem.cpp/h` | 可移动的图像项组件 |
| `SplicingLine.cpp/h` | 拼接线管理组件 |
| `settingwindow.cpp/h` | 设置界面 |
| `aboutdialog.cpp/h` | 关于对话框 |
| `config.hpp` | 配置管理 |
| `enumerate.h` | 枚举定义 |
| `tools.hpp` | 工具函数 |
| `imagepro.hpp` | 图像处理函数 |

### 拼接算法

- **尺寸标准化**：根据配置统一图像尺寸
- **智能对齐**：自动检测和调整图像位置
- **边缘处理**：平滑的图像边界融合
- **质量优化**：保持拼接后的图像质量

## 🔧 配置选项

应用程序支持多种配置选项，通过 `QSettings` 持久化保存：

- `FinishRAWPhicture`: 完成后处理原始图片
- `OpenReverse`: 反向导入文件顺序
- `SaveType`: 保存格式 (PNG/JPG)
- `SplicingType`: 拼接类型 (高到低/低到高/原始)
- `PaddingColorType`: 边缘填充颜色
- `ShrinkType/NarrowType`: 缩放算法

## 🤝 贡献指南

欢迎提交 Pull Request 或 Issue！

### 开发环境设置

1. Fork 此仓库
2. 创建功能分支: `git checkout -b feature/your-feature`
3. 提交更改: `git commit -am 'Add some feature'`
4. 推送分支: `git push origin feature/your-feature`
5. 提交 Pull Request

### 代码规范

- 遵循现有的代码风格
- 添加适当的注释
- 确保新功能有相应的测试

## 📝 更新日志

### v1.0.0 (当前版本)

- 基础图像拼接功能
- 水平和垂直拼接模式
- 拖拽导入支持
- 可视化拼接线编辑
- 多种输出格式支持
- 丰富的配置选项

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 👥 作者

- **JKWTCN** - *初始开发* - [GitHub](https://github.com/JKWTCN)

## 🙏 致谢

- [Qt](https://www.qt.io/) - 跨平台图形界面框架
- [OpenCV](https://opencv.org/) - 计算机视觉库
- 所有贡献者和用户的支持

## 📞 支持

如果您遇到问题或有功能建议，请：

1. 查看 [Issues](https://github.com/JKWTCN/ImageSplicing/issues) 页面
2. 创建新的 Issue 描述问题
3. 或通过邮件联系维护者

---

⭐ 如果这个项目对您有帮助，请给它一个星标！
