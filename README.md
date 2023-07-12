# NUEDC_software
本repository是电赛软件部分的代码仓库，是学习过程中模块的代码存储地点。

我们需要工程做到以下几点：

1. 每个模块有文件夹（最好是一个）、文件夹中**写README.md文档**，可以使使用者方便地上手。
2. 每个模块有样例工程，方便随时上手运行（HAL库，F3Discovery优先）
3. 模块的**所有**相关文件（淘宝店家的资料、Datasheet等）
4. 标注清楚硬件接口，方便实物连接。
5. 标明有哪些文件是该模块必须的文件(.c .h)

我们需要代码做到以下几点：

1. 每个.c .h文件有注释，保证函数体、结构体一定会有**详细注释**，函数起名规范、一看就懂。
2. 使用git管理版本，方便版本迭代、回退和防止意外删除。

**这里我将DDS：AD9834的部分创建了一个样例，Git工程发布在群里，并请接受邀请成为协作者**，请按照格式写代码。因为只有这样，代码才不是写了等于一次性用品，比赛时东抄抄西抄抄一编译就不过。

此外，在软件的学习部分也有必要提醒大家。学习STM32、FPGA过程中，一定要写文档，因为比如定时器、比如ADC、比如PWM，大家并不是、也不可能学不会，但是学过后、过十几天忘记怎么快速配置的可能性还是有，而且很大，并且如果你写了文档，你可以很快速的让组内其他两个同学上手这个模块，例如你们照着我写的DDS模块、加上接线可以非常轻易的上手AD9834，如果彬彬同学把FPGA的文档写了，我们也可以很快速的配置好DDS和FIR的IP核，并且如果彬彬忘记了也可以查阅。老规矩是：我们可以比赛时不写代码，但是不能不会、不懂。

