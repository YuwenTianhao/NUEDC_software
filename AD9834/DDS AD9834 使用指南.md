# DDS AD9834 使用指南

AD9834主频**75MHz**，可以输出的有效波形在37.5MHz（方波），正弦波在10MHz左右失真就不能忽视了，但如果我们愿意在后面加一个截止频率在37.5MHz-70MHz之间的一个低通滤波器，那情况会稍好一点（滤除方波的3、5次谐波）。

![image-20230709160503943](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709160503943.png)

详情通过[ADIsimDDS (Direct Digital Synthesis) | 亚德诺半导体 (analog.com)](https://tools.analog.com/cn/simdds/?fc=37.5M&harmonicDB=-50&mult=1&part=AD9834&rso=111111&sclock=75M&tof=1M&useFilters=0)查看。该芯片通过**SPI**通信，但该SPI并不是纯粹的SPI，而是兼容了多种协议（datasheet写作三线式SPI®接口）。例如：

对于普通单片机的SPI接口：

![image-20230709160114290](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709160114290.png)

其他奇怪的传输协议：

![image-20230709160152386](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709160152386.png)

在STM32 CubeMX中开启SPI，你将看到三条线被开启：

![image-20230709160602397](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709160602397.png)

| 总线 | 作用                       |
| ---- | -------------------------- |
| SCK  | 时钟线                     |
| MISO | master input slaver output |
| MOSI | master output slaver input |

但学过冬令营训练4/5的我们知道，SPI通信还需要一个CS线，在开始通信前CS需要被拉低。AD9834模块上**FSYNC引脚**就充当了CS的作用。故我们还需要使能一个GPIO口，使其充当CS。

当然，三线SPI的强大兼容性使其的通信功能只能变得非常简单，甚至该模块只能接受，不会给主机发任何信息，因此MISO线也就没用任何用处，不连接它了。于是我在网上找到了一个非常好的例程，通过**使能GPIO口**（OUTPUT）和**自写时序**来模拟SPI通信，坏处是没法调控SPI速率了，好处是我们可以使用任意管脚当作通信口，也就能实现把杜邦线聚在一堆（例如都使用GPIO PORTB），更加整洁。

## 简单的示范例程

请运行我已经配置好的DDS_1工程文件，在main函数处修改以下代码，下载到开发板上。

**接线方式：**

![image-20230712160718400](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230712160718400.png)

管脚含义同模块上的标注。

**使用示范：**

```c
HAL_Delay(1000); //等待电平恢复稳定
AD9834_Init();
AD9834_Select_Wave(Sine_Wave);//Sine_Wave Square_Wave Triangle_Wave
AD9834_Set_Freq(FREQ_0, 1000);
 //后续直接选波即可，这里使用的是频率寄存器0
```

> 来自README.MD

只有简单的三个函数使用即可完成DDS的输出

AD9834_Init()：初始化

AD9834_Select_Wave(Sine_Wave)：选择波形/Sine_Wave Square_Wave Triangle_Wave全部是宏定义

AD9834_Set_Freq(FREQ_0, 1000)：选择频率/关于第一个参数（频率寄存器稍复杂，后续会说）。

使用这个简单例程，你就可以马上跑起来DDS，成就感满满。

**如何移植到新的工程？**

AD9834.h（Core/Inc）

![image-20230712161052174](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230712161052174.png)

AD9834.c(Core/Src)

![image-20230712161134953](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230712161134953.png)

做到这里如果不想继续就足够了。

## 重要接口说明

想要继续了解DDS，必须知道其所有接口的名字，也就清楚我们能配置什么，不能配置什么。

以下为模块的原理图：

![image-20230709163905400](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709163905400.png)

| 接口          | 用途                                                         |
| ------------- | ------------------------------------------------------------ |
| FS ADJUST     | 满量程调整控制（电流）（板载直接接R3 6.8k，因此无法修改量程）***** |
| FSYNC （FSY） | 低电平有效控制输入。这是输入数据的帧**同步信号**，当成SPI的CS线。 |
| FSELECT       | **频率选择输入**。FSELECT控制相位累加器中使用的具体频率寄存器(**FREQ0或FREQ1**)。可使用引脚FSELECT或Bit  FSEL来选择要使用的频率寄存器。什么是bit** |
| PSELECT       | **相位选择输入**。PSELECT控制将增加到相位累加器输出的具体相位寄存器(PHASE0或PHASE1)。可使用引脚 PSELECT或Bit PSEL来选择要使用的相位寄存器。 |
| RESET         | 高电平有效数字输入。RESET可使相应的内部寄存器复位至0，以提供**中间电平的模拟输出**。RESET不影响任何可寻址寄存器。 |
| SLEEP         | 高电平有效数字输入。当此引脚处于高电平时，DAC关断。<br />模块中没有引出引脚，但是根据datashhet可知可以通过写入控制寄存器改变该位置状态。![image-20230709164508514](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709164508514.png) |

> *AD9834包含一个高阻抗、电流源10位DAC，能够驱动各种负载。可使用一个外部电阻(RSET)来调节满量程输出电流，以实现最佳电源和外部负载要求。
> $$
> IOUT FULL SCALE = 18 × FSADJUST/RSET  \n
> 
> FSADJUST = 1.15 V(标称值)，RSET = 6.8 kΩ(典型值)。
> $$
> AD9834的最 大输出电压为VDD − 1.5 V。
>
> AD9834的最大电流密度为4 mA，由RSET设置。

### 控制寄存器

![image-20230709170639591](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709170639591.png)

![image-20230709170608450](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709170608450.png)![image-20230709170621171](C:\Users\86139\AppData\Roaming\Typora\typora-user-images\image-20230709170621171.png)

### 其他

> 该芯片还完全能支持各种简单和复杂的调制方案。这些调制方案完全在数字域内实现，使得可以使用DSP技术精确而轻松地实现复杂的调制算法。

## 关于上电的说明

> AD9834上电时，器件应复位。这样可使相应的内部寄存器复位至0， 以提供中间电平的模拟输出。为了避免AD9834初始化时 产生杂散DAC输出，RESET bit/引脚应置1，直至器件准备好开始产生输出。

来自datasheet的建议。

## 目前DDS存在的不满足我们要求的问题

- **输出幅度不可控**
- **两路输出相同**（可能能通过电阻修改幅度）
- 方波也只能输出一种。
