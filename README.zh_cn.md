# Trigger Craft - 触发工坊

使用 C++ 编写的 **帝国时代2决定版** 场景编辑工具。

## 内容列表

- [项目背景](#项目背景)
- [安装](#安装)
- [注意](#注意)
- [使用方式](#使用方式)
    - [现在能做什么](#现在能做什么)
- [API](#api)
- [贡献](#贡献)
- [使用许可](#使用许可)

## 项目背景

从征服者时代的 [AoKTS](https://github.com/mullikine/aokts)，到决定版时代的 [AoE2ScenarioParser](https://github.com/KSneijders/AoE2ScenarioParser)，我们一直以来使用着这些出色的工具来帮助编辑场景。但如今，前者已经不再更新，后者则由于脚本语言的掣肘，不能实现尽如人意的运行效率。

这一项目基于这一初衷建立，使用 Win32 图形界面和 C++ 底层，希望能够成为又一个新的高效、易用、功能强大的场景编辑器。

## 安装

解压，运行，使用。

## 注意

我们会尽量排除意外，但安全起见，使用时 **务必** 备份场景。

设计上面向多语言，但目前仅完成了 **简体中文** 的支持。

本地化文件是从内部的 `string-table` 和外部的 `string.json` 导入的。

## 使用方式

为了减少工作量，暂时只支持最新版本的场景（目前是 1.46）。
### 现在能做什么

- 读取/存储场景。
- 导入/导出场景中的字符串。
- 显示触发树，及下属的条件/效果。
- 复制单个触发到每一位玩家，多人场景常用。
- 显示场景基本信息（仅查看）。
- 显示触发基本信息（仅查看）。
- 增删触发/条件/效果。

## API

暂无。

## 贡献

GUI 部分的许多源代码参考自 [AoKTS](https://github.com/mullikine/aokts)，感谢十余年来历代作者的付出。

场景文件格式参考自 [AoE2ScenarioParser](https://github.com/KSneijders/AoE2ScenarioParser) 项目内容。

### 如何贡献
欢迎加入——不如说是亟待加入，[提出 Issue](https://github.com/MegaDusknoir/AoE2TriggerCraft/issues/new) 或者提交一个 Pull Request。

## 使用许可

[GNU General Public License v3.0](LICENSE)