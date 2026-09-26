# Third-party notices

本仓库包含用于技术研究和工程分析的课程示例、芯片厂商支持文件与器件资料衍生图。

## 课程示例

`projects/**/course/` 保存从配套资料中筛选出的原始工程文件。迁移时只移除了编译产物、缓存和无关工具，保留文件内容与文件名；对应 SHA-256 记录见 [`docs/course-source-sha256.csv`](docs/course-source-sha256.csv)。

## 芯片厂商组件

工程中使用的 CMSIS、GD32F4 标准外设库、STM32F4 标准外设库与 STM32 HAL 文件，其版权和许可归 ARM、GigaDevice、STMicroelectronics 及相应权利人所有。相关文件继续适用其原有声明。

## 硬件资料与图片

开发板原理图和器件资料归原发布方所有。仓库仅保留少量用于接口说明的渲染图，来源记录在 [`assets/images/SOURCES.md`](assets/images/SOURCES.md)。

## 仓库新增内容

本仓库新增的目录组织、工程导航、技术分析和 Markdown 文档采用根目录 `LICENSE`。第三方文件不因收录到本仓库而改变原有授权。

