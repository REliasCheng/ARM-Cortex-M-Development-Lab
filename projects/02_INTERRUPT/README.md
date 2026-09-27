# EXTI 与 NVIC | EXTI and NVIC

## 代表工程 | Selected Project

[`exti-callback/course/`](exti-callback/course/) 包含 GPIO 输入配置、EXTI Line 路由、NVIC 优先级、Pending Flag 处理和回调接口。按键路径使用 SysTick 计时，将机械消抖与原始边沿检测分开。

## 事件流 | Event Flow

```text
Button edge
    ↓
GPIO input → EXTI line → NVIC → ISR
                                  ↓
                         clear pending flag
                                  ↓
                         callback / LED action
```

Keil 工程位于 `course/Project/GD32F407.uvprojx`。

## 硬件关系 | Hardware Relationship

GPIO 输入通过系统配置映射到 EXTI Line。EXTI 检测指定边沿并向 NVIC 提交中断请求；处理函数清除 Pending Flag 后再分发注册动作。

## 关键接口 | Key Interfaces

- [`EXTI.h`](exti-callback/course/Library/EXTI.h)：初始化与回调接口。
- [`EXTI.c`](exti-callback/course/Library/EXTI.c)：Line 路由、触发方式和 NVIC 配置。
- [`EXTI_config.h`](exti-callback/course/Library/EXTI_config.h)：工程级中断映射。
- [`gd32f4xx_it.c`](exti-callback/course/User/gd32f4xx_it.c)：异常处理函数。
