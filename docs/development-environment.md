# 开发环境 | Development Environment

## 工程工具 | Tooling

- Keil MDK-ARM 工程文件（`.uvprojx`）
- GD32F4 Device Pack 与标准外设库
- STM32F4 CMSIS / 标准外设库
- STM32CubeMX 配置文件（`.ioc`）
- 面向 Keil MDK 生成的 STM32 HAL 源码

当前默认分支不包含 Keil、CubeMX/HAL 或厂商示例工程。以下内容仅说明未来从官方渠道建立个人工程时的工具链边界。

## Host 控制核心

当前默认分支提供原创 C11 控制源码、Mock 测试和 Demo。普通 PC 的严格 GCC/Clang 构建与 ASan/UBSan 流程见 [Timer-to-Sample Control](control-foundation.md) 和仓库 CI。它们验证应用状态、队列边界和错误传播，**不是** GD32/STM32 目标编译或板端运行。原始资产与许可取舍见[来源记录](source-provenance.md)。

## 打开 GD32 工程

1. 安装 Keil MDK-ARM。
2. 安装工程目标所需的 GD32F4 Device Pack。
3. 从授权渠道取得器件支持包和启动组件，并新建个人 `.uvprojx` 工程。
4. 核对目标器件、启动文件和 Include Paths。
5. 下载前选择实际使用的调试器与 Flash Algorithm。

## 打开 CubeMX/HAL 工程

1. 使用兼容版本的 STM32CubeMX 新建 `.ioc`，配置引脚与时钟。
2. 生成目标工具链工程，并在 Keil 中打开生成的工程。
3. 对照 `.ioc` 中的 STM32F407 目标，核对 Keil Device 与 Device Pack。

GD32 与 STM32 的器件包、启动文件和外设库不能混用。新建工程时必须使 `.ioc`、源码目标、Device Pack 与实际芯片一致。

## 构建输出 | Build Outputs

`.o`、`.d`、`.crf`、`.map`、`.axf`、`.hex`、`.bin`、`.lst`、`.lnp`、`.dep` 与 IDE 用户状态文件不进入版本控制，本地重新构建时由工具链生成。

## 硬件检查 | Hardware Checks

下载前核对 MCU 丝印、供电、BOOT 状态、SWD 接线和串口电平。串口号与调试器配置取决于当前连接设备，不在仓库中固定。
