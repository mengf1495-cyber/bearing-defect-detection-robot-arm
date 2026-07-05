# 基于 GD32H7 双 MCU 的嵌入式 AI 轴承缺陷检测与机械臂分拣系统

## 项目简介

本系统基于 **GigaDevice GD32H759IMK6**（ARM Cortex-M7）和 **Infineon XMC7100** 双 MCU 架构，实现了工业传送带上的**轴承表面缺陷 AI 检测**与**自动分拣**功能。

- **GD32H759IMK6**：运行 4 层卷积神经网络（CNN），通过 OV2640 摄像头采集 96×96 灰度图像，实时推理判断轴承是否存在缺陷
- **XMC7100**：控制传送带直流电机、NPN 激光传感器，并通过串行总线舵机控制器执行翻面和剔除动作

两块 MCU 之间通过 UART3（115200 bps）使用自定义 `$CMD!` 帧协议进行通信。

---

## 系统架构

```
+---------------------------+       UART3 (115200)       +---------------------------+
|     GD32H759IMK6          |   <--- $CMD! / $RESP! ---> |     XMC7100               |
|  (AI 推理主控)             |                            |  (电机/舵机协控制器)        |
|                           |                            |                           |
| - OV2640 摄像头 (96×96)   |   发送指令:                  | - DC 传送带电机控制        |
| - AI 缺陷检测推理          |   PING, MOTOR_ON/OFF,      | - NPN 激光传感器读取       |
| - 图像增强 (黑电平/增益/γ) |   FLIP, BAD, RESET,        | - 舵机总线 (UART1)        |
| - 检测状态机 (5 阶段)      |   STATUS?                  | - 动作序列引擎 (9步/序列)  |
| - OLED 实时显示            |                            | - 3s 心跳超时安全保护      |
| - UART2 图像流输出         |   接收上报:                  |                           |
|                           |   PONG, SENSOR:0/1,       |                           |
|                           |   FLIP_START, BAD_START,  |                           |
|                           |   DONE, BUSY, ERR         |                           |
+---------------------------+                            +---------------------------+
         |
         | UART2 (115200) — 实时图像流
         v
+-----------------------+
|  PC 上位机 (Python)    |
|  - 实时摄像头预览       |
|  - AI 结果叠加显示      |
|  - 数据集采集工具       |
+-----------------------+
```

### 检测流水线

```
PHASE_FIRST → PHASE_FLIPPING → PHASE_SECOND → PHASE_REJECT → PHASE_DONE
  (正面检测)     (翻面)         (反面检测)      (剔除缺陷品)     (完成)
```

- 轴承经传送带到达检测工位，激光传感器触发
- 正面拍摄 → AI 推理 → 如有缺陷直接剔除，否则翻面
- 反面拍摄 → AI 推理 → 如有缺陷剔除，否则放行

---

## 目录结构

```
opensource/
├── README.md                          # 本文件
│
├── GD32H7/                            # 主控 MCU 固件
│   ├── Core/                          # 核心应用层
│   │   ├── Include/                   # ● nn_model_configure.h  模型配置
│   │   │                              # ● nn_model_benchmark.h  基准测试
│   │   │                              # ● systick.h             系统滴答
│   │   └── Source/                    # ● main.c                主状态机入口
│   │                                  # ● nn_model_configure.c  模型参数 (34KB)
│   │                                  # ● nn_model_benchmark.c  性能基准
│   │                                  # ● nn_model_report.c     模型报告
│   │                                  # ● systick.c             系统滴答
│   ├── Hardware/                      # 硬件驱动层
│   │   ├── RobotArm/                  # 机械臂控制核心
│   │   │   ├── remote_io.c/h          #   GD32↔XMC UART 通信层
│   │   │   ├── bsp_robot_arm.c/h      #   机械臂统一 API
│   │   │   ├── robot_arm_entry.c/h    #   机械臂入口/初始化
│   │   │   ├── robot_arm_board.h      #   完整引脚规划
│   │   │   ├── y_kinematics/          #   逆运动学解算器
│   │   │   ├── y_global/              #   全局定义与配置
│   │   │   ├── resource/              #   字体/图像资源
│   │   │   ├── app_ps2.c/h            #   PS2 手柄控制
│   │   │   ├── app_uart.c/h           #   UART 应用层
│   │   │   ├── app_sensor.c/h         #   传感器应用层
│   │   │   ├── robot_arm_adc.c/h      #   电池电压 ADC
│   │   │   ├── robot_arm_buzzer.c/h   #   蜂鸣器
│   │   │   ├── robot_arm_flash.c/h    #   W25Q64 Flash 存储
│   │   │   ├── robot_arm_gpio.c/h     #   GPIO 扩展
│   │   │   ├── robot_arm_port.c/h     #   端口管理
│   │   │   ├── robot_arm_soft_i2c.c/h #   软件 I2C
│   │   │   ├── robot_arm_time.c/h     #   定时器
│   │   │   ├── robot_arm_ultrasonic.c/h # 超声波测距
│   │   │   ├── main.h                 #   总头文件
│   │   │   └── y_*.h                  #   辅助头文件
│   │   ├── OV2640/                    # OV2640 摄像头驱动
│   │   │   ├── bsp_ov2640.c/h
│   │   │   └── bsp_ov2640_init_table.h
│   │   ├── OLED/                      # OLED 显示驱动
│   │   │   ├── bsp_oled.c/h
│   │   │   └── bsp_lcd_font.c/h
│   │   ├── LCD/                       # RGB LCD 驱动
│   │   │   ├── bsp_rgb_lcd.c/h
│   │   │   ├── bsp_rgb_pwm.c/h
│   │   │   └── bsp_lcd_font.c/h
│   │   ├── LED/                       # LED 驱动
│   │   │   └── bsp_led.c/h
│   │   └── I2C/                       # 软件 I2C
│   │       └── bsp_software_i2c.c/h
│   ├── MCU_driver/                    # MCU 外设抽象层
│   │   ├── inc/                       # GPIO, DMA, Timer, EXMC, I2C
│   │   └── src/
│   └── Public_driver/                 # 公共工具层
│       ├── inc/                       # driver_public.h, driver_malloc.h, gd_log.h
│       └── src/                       # driver_public.c, driver_malloc.c, gd_log.c
│
├── XMC7100/                           # 协处理器固件
│   ├── README.md                      #   移植指南
│   ├── main.c                         #   主状态机 & 命令分发
│   ├── hal_platform.h                 #   硬件抽象层 (移植接口)
│   ├── proto_parser.c/h               #   $...! 协议帧解析器
│   ├── proto_responder.c/h            #   响应帧构建器
│   ├── sequence_engine.c/h            #   动作序列引擎 (FLIP/BAD)
│   ├── motor.c/h                      #   直流电机控制
│   ├── sensor.c/h                     #   NPN 激光传感器
│   └── servo_bus.c/h                  #   舵机总线通信
│
├── Model/                             # AI 模型文件
│   ├── bearing_model.tflite           # 部署用 TFLite 模型 (int8 量化)
│   └── bearing_model_clean.h5         # Keras H5 模型 (402 KB)
│
├── Tools/                             # PC 端工具
│   ├── camera_viewer.py               # PyQt5 上位机 (实时预览+数据采集)
│   ├── test_model.py                  # TFLite 模型测试脚本
│   ├── test_h5.py                     # Keras 模型测试脚本
│   ├── check_class_order.py           # 数据集类别检查
│   ├── frame.pgm                      # 测试样本图像
│   └── frame.png                      # 测试样本图像
│
└── Dataset/                           # 训练数据集 (示例)
    ├── normal/                        # 正常轴承样本 (20张)
    └── defect/                        # 缺陷轴承样本 (20张)
```

---

## AI 模型

### 网络结构

```
CONV_2D → MAX_POOL_2D → CONV_2D → MAX_POOL_2D →
CONV_2D → MAX_POOL_2D → CONV_2D → MAX_POOL_2D →
GLOBAL_MEAN → FULLY_CONNECTED → FULLY_CONNECTED → SOFTMAX
```

- **输入**：96×96 灰度图，int8 量化（zero_point=-128, scale=1.0）
- **输出**：2 维向量 `[class_0(normal), class_1(defect)]`
- **判定规则**：`class_0 - class_1 >= 250` → 缺陷，否则 → 正常
- **参数量**：~34KB 权重数据（`model_paras_arr[34156]`）
- **运行时内存**：~184KB 峰值缓冲区

### 图像预处理

- 黑电平校正
- 模拟增益 (gain)
- Gamma 校正
- 有效帧判断（均值/动态范围检查）

---

## 通信协议

GD32H7（主）→ XMC7100（从），UART3, 115200 8N1。

### 命令帧格式：`$CMD!`

| 命令 | 说明 |
|------|------|
| `$PING!` | 心跳（3秒超时触发安全模式） |
| `$MOTOR_ON!` | 启动传送带电机 |
| `$MOTOR_OFF!` | 停止传送带电机 |
| `$FLIP!` | 执行翻面动作序列（9步, ~16秒） |
| `$BAD!` | 执行剔除动作序列（9步, ~13秒） |
| `$RESET!` | 复位到初始状态 |
| `$STATUS?!` | 查询当前状态 |

### 响应帧格式：`$RESPONSE!`

| 响应 | 说明 |
|------|------|
| `$PONG!` | 心跳应答 |
| `$SENSOR:0!` / `$SENSOR:1!` | 激光传感器状态上报 |
| `$FLIP_START!` / `$BAD_START!` | 动作开始 |
| `$DONE!` | 动作完成 |
| `$BUSY!` | 忙（拒绝新命令） |
| `$ERR!` | 错误 |
| `$STOPPED!` | 安全停止 |

---

## 硬件依赖

### 主控板 (GD32H759IMK6)
- **MCU**: GD32H759IMK6 (Cortex-M7 @ 600MHz)
- **摄像头**: OV2640 (DCI 接口)
- **显示**: OLED (I2C) + RGB LCD
- **存储**: W25Q64 Flash (SPI)
- **其他**: 超声波模块、蜂鸣器、电池 ADC、PS2 手柄接口

### 协处理器 (XMC7100)
- **MCU**: Infineon XMC7100 (Cortex-M7)
- **电机**: 直流电机 (PWM + 方向控制)
- **传感器**: NPN 激光传感器 (常开, 低电平触发)
- **舵机**: 6 路串行总线舵机 (UART1, `#xxxPxxxxTxxxx!` 协议)

### 引脚规划
详见 `GD32H7/Hardware/RobotArm/robot_arm_board.h`（含完整 2×13 扩展接口引脚复用方案）。

---

## 编译指南

### GD32H7 固件

1. 从 [GigaDevice 官网](https://www.gd32mcu.com) 下载 **GD32H7xx 标准外设库** 和 **GD NN 推理库**
2. 将 `Firmwares/`、`Middlewares/AI/`、`Utilities/` 放置到工程对应目录
3. 使用以下 IDE 之一打开工程：
   - **GD32EBuilder** (Eclipse) — `GD32EBuilder/.project`
   - **IAR EWARM** — `GD32H759-EWARM/GD32H759I_EVAL.eww`
   - **Keil MDK** — `GD32H759-MDK-ARM/GD32H759I_EVAL.uvprojx`
4. 添加本仓库 `GD32H7/` 下所有 `.c` 文件到工程源码列表
5. 编译下载

### XMC7100 固件

详见 `XMC7100/README.md`。核心步骤：

1. 使用 Infineon DAVE / ModusToolbox 创建新工程
2. 根据目标硬件修改 `hal_platform.h` 中的引脚定义
3. 添加 `XMC7100/` 下所有 `.c` 文件
4. 使能看门狗
5. 编译下载

---

## 上位机使用

```bash
pip install pyqt5 pyserial numpy opencv-python pillow
python Tools/camera_viewer.py
```

连接 GD32H7 的 UART2 串口，即可实时查看摄像头画面和 AI 推理结果，并支持一键保存图像到数据集。

---

## 模型测试

```bash
# 测试 TFLite 模型
python Tools/test_model.py

# 测试 Keras H5 模型
python Tools/test_h5.py
```

---

## 许可证

本项目代码采用 **BSD 3-Clause License** 开源。

依赖的第三方库：
- **CMSIS / CMSIS-NN**: Apache 2.0 License (ARM)
- **GD32H7 标准外设库**: GigaDevice BSD License
- **GD NN 推理引擎**: GigaDevice 专有许可（以 `.lib`/`.a` 形式提供）

---

## 作者

CIMC GD Embedded AI Team

---

## 免责声明

本项目为竞赛/研究用途的嵌入式 AI 演示系统。工业生产环境部署前需进行充分的安全评估和可靠性测试。
