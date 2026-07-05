# 面向工业质检的嵌入式AI双面视觉缺陷检测与自动分拣系统

## 项目简介

本系统实现工业传送带上的**轴承表面缺陷 AI 检测**与**自动分拣**。采用双 MCU 架构：GD32H759IMK6 负责 AI 视觉推理，**XMC7100 作为底层执行控制器**接管电机、传感器、舵机等全部实时 I/O，两者通过 UART3 自定义帧协议解耦。

---

## 系统架构

```
GD32H759IMK6 (AI主控)               XMC7100 (执行控制器)
┌────────────────────────┐   UART3   ┌──────────────────────────────┐
│ 摄像头采集 + CNN推理    │ <$CMD! -->│ 电机·传感器·舵机·动作序列引擎 │
│ (非实时，上层决策)      │ <--$RESP! │ (硬实时，底层执行)             │
└───────────┬────────────┘           └──────────────────────────────┘
            │ UART2 图像流
            v
      PC 上位机 (Python)
```

### 检测流水线

```
正面检测 → 翻面 → 反面检测 → 剔除/放行
```

---

## XMC7100 执行控制器（核心开源内容）

### 设计理念

将**实时性敏感的底层 I/O** 从 AI 主控中剥离，交由独立 MCU 专门处理，实现：

- **职责分离**：GD32H7 专注 AI 推理（计算密集型），XMC7100 专注实时控制（I/O 密集型）
- **安全隔离**：XMC7100 内置 3 秒心跳超时 → 自动进入安全模式，即使 GD32H7 死机也不会失控
- **易于移植**：全部硬件操作抽象为 `hal_platform.h` 中的 12 个函数，换 MCU 只需重新实现这 12 个接口
- **可独立测试**：XMC7100 主循环结构允许脱离 GD32H7，通过串口工具直接发送指令调试

### 文件结构

```
XMC7100/
├── main.c                 # 主状态机 + 命令分发 (核心)
├── hal_platform.h         # 硬件抽象层 — 唯一需移植的文件
├── proto_parser.c/.h      # $CMD! 帧协议解析器
├── proto_responder.c/.h   # 响应帧构建器
├── sequence_engine.c/.h   # 9步FLIP / 9步BAD 动作序列引擎
├── motor.c/.h             # 直流电机 GPIO 控制
├── sensor.c/.h            # NPN 激光传感器读取 + 去抖
├── servo_bus.c/.h         # 串行总线舵机 UART 通信
└── README.md              # 详细移植指南
```

### 主状态机

```
 INIT ──→ IDLE ──→ FLIPPING ──→ IDLE
            │          │
            │          └──→ (完成) ──→ IDLE
            │
            └──→ REJECTING ──→ IDLE
            │          │
            │          └──→ (完成) ──→ IDLE
            │
            └──→ SAFE_MODE  (心跳超时触发，需 RESET 恢复)
```

- **INIT**：初始化硬件，舵机归零
- **IDLE**：等待命令，周期性接收 PING，上报传感器状态
- **FLIPPING**：执行 9 步翻面序列，拒绝新命令（返回 BUSY）
- **REJECTING**：执行 9 步剔除序列，拒绝新命令（返回 BUSY）
- **SAFE_MODE**：3 秒未收到 PING 自动进入，停止电机，禁止舵机动作

### 硬件抽象层 (`hal_platform.h`)

这是整个 XMC7100 固件可移植性的关键。定义了 12 个硬件接口函数：

| 类别 | 函数 | 功能 |
|------|------|------|
| GPIO 输出 | `hal_gpio_output_init(port, pin)` | 初始化推挽输出引脚 |
| GPIO 输入 | `hal_gpio_input_pullup_init(port, pin)` | 初始化上拉输入引脚 |
| GPIO 写 | `hal_gpio_set/reset(port, pin)` | 置高/置低 |
| GPIO 读 | `hal_gpio_read(port, pin)` | 读取引脚电平 |
| UART | `hal_uart_init(handle, baud)` | 初始化串口 |
| UART | `hal_uart_send_byte/hal_uart_send_str(...)` | 发送字节/字符串 |
| UART | `hal_uart_recv_byte/hal_uart_rx_available(...)` | 接收字节/查询缓冲区 |
| 定时器 | `hal_timer_millis()` | 获取系统毫秒计数 |
| 定时器 | `hal_timer_delay_ms(ms)` | 毫秒级阻塞延时 |

→ 移植到其他 MCU（STM32/ESP32 等）只需实现这 12 个函数即可。详见 `XMC7100/README.md`。

### 通信协议

UART3, 115200 8N1。GD32H7（主）发送 `$CMD!` 帧，XMC7100（从）执行并返回 `$RESPONSE!`。

**命令集：**

| 命令 | 说明 | 响应 |
|------|------|------|
| `$PING!` | 心跳保活（3秒超时→安全模式） | `$PONG!` |
| `$MOTOR_ON!` | 启动传送带电机 | `$OK!` |
| `$MOTOR_OFF!` | 停止传送带电机 | `$OK!` |
| `$FLIP!` | 执行翻面序列（~16秒） | `$FLIP_START!` → `$DONE!` |
| `$BAD!` | 执行剔除序列（~13秒） | `$BAD_START!` → `$DONE!` |
| `$RESET!` | 复位舵机到初始位置 | `$OK!` |
| `$STATUS?!` | 查询当前状态 | `$IDLE!` / `$BUSY!` / `$SAFE!` |

**主动上报：**

| 事件 | 帧内容 | 触发条件 |
|------|--------|----------|
| 传感器触发 | `$SENSOR:1!` | NPN 激光被遮挡（轴承到达） |
| 传感器释放 | `$SENSOR:0!` | NPN 激光恢复（轴承离开） |

### 动作序列引擎 (`sequence_engine.c`)

每个序列由 9 个步骤组成，每步定义 6 个舵机的位置和运动时间，通过 UART1 发送 `#xxxPxxxxTxxxx!` 指令到外部舵机控制器。

```
FLIP 序列 (翻面):  9 步 × 6 舵机 ~16秒
BAD 序列 (剔除):   9 步 × 6 舵机 ~13秒
```

### 传感器处理

NPN 激光传感器（常开，低电平触发），5ms 软件去抖，边缘检测自动上报状态变化。

### 电机控制

直流电机通过 3 根 GPIO 控制：PWMA（PWM 调速）、AIN1/AIN2（方向）。`motor_on()` 正转驱动传送带，`motor_off()` 停止。

---

## GD32H759IMK6 AI 主控（简要）

GD32H7 运行 Cortex-M7 @ 600MHz，负责：

- **摄像头采集**：OV2640 96×96 灰度图，含黑电平校正、增益、Gamma 校正
- **AI 推理**：4 层 CNN（CONV→POOL×4 → MEAN → FC×2 → SOFTMAX），int8 量化，~34KB 权重
- **主状态机**：PHASE_FIRST → PHASE_FLIPPING → PHASE_SECOND → PHASE_REJECT → PHASE_DONE
- **显示输出**：OLED 实时显示 FPS、状态、AI 置信度
- **上位机通信**：UART2 实时图像流输出

### 文件结构

```
GD32H7/
├── Core/               # 主入口 (main.c) + AI模型配置
├── Hardware/
│   ├── RobotArm/       # 远程IO层 (remote_io — 对XMC7100的封装)
│   ├── OV2640/         # 摄像头驱动
│   ├── OLED/           # 显示驱动
│   └── ...             # LED/LCD/I2C 等
├── MCU_driver/         # 外设抽象层
└── Public_driver/      # 工具层 (malloc/log)
```

---

## AI 模型

```
CONV → POOL → CONV → POOL → CONV → POOL → CONV → POOL → MEAN → FC → FC → SOFTMAX
```

| 参数 | 值 |
|------|-----|
| 输入 | 96×96 灰度，int8 |
| 输出 | [normal_score, defect_score] |
| 判定 | defect_score − normal_score ≥ 250 → 缺陷 |
| 权重大小 | ~34KB |
| 运行时峰值内存 | ~184KB |

详见 `Model/` 目录。

---

## 目录结构

```
opensource/
├── README.md
│
├── XMC7100/                    # ★ 核心 — 执行控制器固件
│   ├── main.c                  #   主状态机 (状态图见上文)
│   ├── hal_platform.h          #   硬件抽象层 (12个移植接口)
│   ├── proto_parser.c/h        #   协议解析器
│   ├── proto_responder.c/h     #   响应构建器
│   ├── sequence_engine.c/h     #   动作序列引擎
│   ├── motor.c/h               #   电机控制
│   ├── sensor.c/h              #   传感器读取
│   ├── servo_bus.c/h           #   舵机总线
│   └── README.md               #   移植指南
│
├── GD32H7/                     # AI 主控固件 (简要)
│   ├── Core/                   #   主入口 + 模型配置
│   ├── Hardware/RobotArm/      #   远程 IO 层 + 机械臂驱动
│   ├── Hardware/OV2640/        #   摄像头驱动
│   ├── Hardware/OLED/          #   显示驱动
│   ├── MCU_driver/             #   外设抽象层
│   └── Public_driver/          #   工具层
│
├── Model/                      # AI 模型
│   ├── bearing_model.tflite
│   └── bearing_model_clean.h5
│
├── Tools/                      # PC 工具
│   ├── camera_viewer.py        #   上位机
│   ├── test_model.py           #   模型测试
│   └── test_h5.py
│
└── Dataset/                    # 样本数据
    ├── normal/  (20张)
    └── defect/  (20张)
```

---

## 快速开始

### XMC7100 固件

1. 使用 Infineon DAVE / ModusToolbox 创建新工程
2. 添加 `XMC7100/` 下所有 `.c` 文件
3. 根据目标硬件修改引脚宏（`motor.c`、`sensor.c`、`servo_bus.c`、`main.c`）
4. 在 `main.c` 主循环中添加看门狗喂狗
5. 编译下载

→ 详细移植说明见 `XMC7100/README.md`

### GD32H7 固件

1. 安装 GD32H7xx 标准外设库和 GD NN 推理库（GigaDevice 官网获取）
2. 使用 Keil / IAR / GD32EBuilder 打开工程
3. 添加 `GD32H7/` 下源码到工程列表
4. 编译下载

### PC 上位机

```bash
pip install pyqt5 pyserial numpy opencv-python pillow
python Tools/camera_viewer.py
```

---

## 硬件依赖

| 模块 | XMC7100 控制 | GD32H7 控制 |
|------|:---:|:---:|
| 直流传送带电机 (PWM+方向) | ✅ | — |
| NPN 激光传感器 | ✅ | — |
| 6 路串行总线舵机 (UART) | ✅ | — |
| OV2640 摄像头 (DCI) | — | ✅ |
| OLED 显示屏 (I2C) | — | ✅ |
| RGB LCD | — | ✅ |
| W25Q64 Flash | — | ✅ |
| 超声波模块 / 蜂鸣器 / ADC | — | ✅ |

---

## 作者

精检智控

---

## 许可证

本项目代码采用 **BSD 3-Clause License**。

依赖第三方库：CMSIS (Apache 2.0)、GD32H7 标准外设库 (BSD)、GD NN 推理引擎 (GigaDevice 专有，以 .lib 形式提供)。

---

## 免责声明

本项目为竞赛/研究用途嵌入式 AI 演示系统。工业生产环境部署前需充分安全评估和可靠性测试。
