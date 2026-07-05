# XMC7100 电机与机械臂控制器固件

## 角色

XMC7100 负责电机控制和机械臂动作指令输出，通过 UART 接收 GD32H7 的命令。

## 通信协议

GD32H7 (Master) 通过 UART3 (115200 8N1) 发送 `$CMD!` 帧给 XMC7100 (Slave)。
XMC7100 执行后返回 `$RESPONSE!` 帧。

详见项目 plan 文件中的完整协议说明。

## 移植指南

### 1. 实现 hal_platform.h 的 12 个函数

这是唯一需要移植的文件。参考 Infineon XMC HAL 文档实现以下函数：

| 函数 | XMC HAL 参考 |
|------|-------------|
| `hal_gpio_output_init()` | `XMC_GPIO_SetMode(port, pin, XMC_GPIO_MODE_OUTPUT_PUSH_PULL)` |
| `hal_gpio_input_pullup_init()` | `XMC_GPIO_SetMode(port, pin, XMC_GPIO_MODE_INPUT_PULL_UP)` |
| `hal_gpio_set()` | `XMC_GPIO_SetOutputHigh(port, pin)` |
| `hal_gpio_reset()` | `XMC_GPIO_SetOutputLow(port, pin)` |
| `hal_gpio_read()` | `XMC_GPIO_GetInput(port, pin)` |
| `hal_uart_init()` | `XMC_UART_CH_Init()` + 引脚配置 |
| `hal_uart_send_byte()` | `XMC_UART_CH_Transmit()` |
| `hal_uart_send_str()` | 循环调用 `_send_byte()` |
| `hal_uart_recv_byte()` | 从内部环形缓冲区读取 |
| `hal_uart_rx_available()` | 返回环形缓冲区中可用字节数 |
| `hal_timer_millis()` | `XMC_SYSTIMER_GetTimerValue()` 或其他 SysTick |
| `hal_timer_delay_ms()` | 基于 SysTick 的阻塞延迟 |

### 2. 设置引脚宏

在以下文件中更新引脚定义：
- `motor.c` — MOTOR_PWMA/AIN1/AIN2
- `sensor.c` — SENSOR_PORT/PIN
- `servo_bus.c` — SERVO_TX/RX 引脚
- `main.c` — GD32 UART 通信的 TX/RX 引脚

### 3. 启用看门狗

在 `main.c` 的 `while(1)` 循环中添加看门狗喂狗代码。

### 4. 创建 XMC7100 工程

使用 Infineon DAVE 或 ModusToolbox 创建新工程，添加本目录下所有 `.c` 文件。

## 目录结构

```
XMC7100/
├── README.md
├── main.c              # 主状态机和命令分发
├── hal_platform.h      # 硬件抽象层定义（移植接口）
├── proto_parser.c/.h   # 命令帧解析器 ($...!)
├── proto_responder.c/.h # 响应帧构建器
├── sequence_engine.c/.h # 动作序列引擎 (FLIP 10步 / BAD 9步)
├── motor.c/.h          # 直流电机 GPIO 控制
├── sensor.c/.h         # NPN 激光传感器读取
└── servo_bus.c/.h      # UART 舵机总线通信
```
