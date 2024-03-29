什么是操作系统？

- Windows，UNIX，Mac OS，Android，IOS，...
- 操作系统是直接运行于硬件之上的计算机程序
- 操作系统用于管理和控制计算机的硬件资源
- 操作系统为用户软件的开发提供必要的服务和接口
- ......

## BIOS - Base Input & Output System

- BIOS 是计算机上电后**第一个运行的程序**
- BIOS 首先检测硬件状态，检测通过后立即进行硬件初始化
- BIOS 会在内存中**建立中断向量表**（提供硬件访问方法）
- BIOS 最后将控制权交由**主引导程序**执行

### 注意！注意！注意！

- BIOS 不是软件（**Software**），而是固件（**firmware**）！
- 固件是固化于硬件中的程序，在硬件出厂前已经烧写固定。

### BIOS的运行机制

- **BIOS 存储于ROM中**，地址映射为0xF0000 - 0xFFFFF(**实地址**)
- BIOS 的入口地址为：**0xFFFF0**
- 硬件电路的特殊设计使得：**开机后，CPU从0xFFFF0处开始执行**

### BIOS最后的使命

- **按照用户设置扫描各个存储介质**（光驱、软驱、U盘等）
- 发现主引导区后，**将主引导区中的主引导程序载入内存**
- **主引导程序在内存中的入口地址为0x7c00**
- 将控制权交由主引导程序执行（**jmp 0x7c00**）
