# 开发环境 | Development Environment

## 工程工具 | Tooling

- Keil MDK-ARM 工程文件（`.uvprojx`）
- GD32F4 Device Pack 与标准外设库
- STM32F4 CMSIS / 标准外设库
- STM32CubeMX 配置文件（`.ioc`）
- 面向 Keil MDK 生成的 STM32 HAL 源码

仓库保留的是 Keil 与 CubeMX/HAL 工程，没有把厂商资料包中的 CubeIDE 模板作为项目入口。

## 打开 GD32 工程

1. 安装 Keil MDK-ARM。
2. 安装工程目标所需的 GD32F4 Device Pack。
3. 打开项目 `course/Project/` 下的 `.uvprojx`。
4. 核对目标器件、启动文件和 Include Paths。
5. 下载前选择实际使用的调试器与 Flash Algorithm。

## 打开 CubeMX/HAL 工程

1. 使用兼容版本的 STM32CubeMX 打开 `.ioc`，查看引脚与时钟配置。
2. 使用 Keil 打开 `course/MDK-ARM/` 下的工程。
3. 对照 `.ioc` 中的 STM32F407 目标，核对 Keil Device 与 Device Pack。

部分名称含 STM32 的原始 Keil 工程仍选择 GD32 Device Pack。仓库保持原配置不变，打开前需要把 `.ioc`、源码目标和 Keil 设备选择放在一起核对。

## 构建输出 | Build Outputs

`.o`、`.d`、`.crf`、`.map`、`.axf`、`.hex`、`.bin`、`.lst`、`.lnp`、`.dep` 与 IDE 用户状态文件不进入版本控制，本地重新构建时由工具链生成。

## 硬件检查 | Hardware Checks

下载前核对 MCU 丝印、供电、BOOT 状态、SWD 接线和串口电平。串口号与调试器配置取决于当前连接设备，不在仓库中固定。
