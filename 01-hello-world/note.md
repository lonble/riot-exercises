# 构建命令

1. `make all`生成ELF文件，默认命令
2. `make flash`把ELF文件（转换格式后）写入板子。`make flash`会自动把板子设置为bootloader模式，写入完成后自动切回默认模式
3. `make term`使用`pyterm`连接到板子
4. `make flash`依赖`make all`，如果只写入不编译，使用`make flash-only`
5. `make clean`清空`bin`目录
6. `make compile-commands`生成针对当前构建的编译参数（IDE集成）

有时`make flash`不成功，是因为板子切换为bootloader模式后Linux需要时间识别和挂载，然后才能被写入程序识别。当前配置的等待时间为3秒，适当延长即可解决问题。

板子的配置在
`RIOT/boards/common/adafruit-nrf52-bootloader/Makefile.include:64`

总配置在
`RIOT/makefiles/tools/usb_board_reset.mk:19`

# 构建行为

写入的本质是覆盖板子一级目录下的`CURRENT.UF2`文件。

默认使用`/dev/ttyACM0`来访问串口设备，会使用该接口切换模式，但不会自动挂载。

使用`uf2conv`进行写入。

脚本非常依赖用户手动配置的自动挂载，切换模式后，脚本只会在几个固定目录（比如`/mnt`、`/media`、`/run/media/<user>`）下查找，如果这些目录下的某个目录的一级目录存在`INFO_UF2.TXT`文件，则认为是板子。

# 构建系统

项目的`Makefile`有三个必须的变量，以及必须`include`RIOT的`Makefile.include`：

```makefile
# name of your application
APPLICATION = hello-world

# If no BOARD is found in the environment, use this default:
BOARD ?= adafruit-feather-nrf52840-sense

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

include $(RIOTBASE)/Makefile.include
```

如果没有指定`SRC`变量，默认编译项目一级目录下的所有`.c`文件。如果指定了`SRC`变量，则只编译该变量指定的文件。指定文件可以使用相对目录。

```makefile
# source files
SRC += main.c
SRC += subdir/source.c
```

头文件查找目录只有项目一级目录和RIOT自己的各种`include`目录。目前没找到怎么处理目标依赖。

模块必须添加到`Makefile`才能使用。

```makefile
# required modules
USEMODULE += ztimer
USEMODULE += ztimer_sec
```

# 板子行为

## 模式

分为**默认模式**和**bootloader模式**。

刚通电处于默认模式，写入的程序自动执行。Reset按钮会让板子重新进入默认模式，重新运行程序。

板子处于bootloader模式才能写入程序，此模式**不会**运行程序。按两下Reset按钮进入bootloader模式。

bootloader模式会额外添加一个块设备，一般是`/dev/sda`，我们需要把它挂载到某个写入脚本可识别的路径。

## 指示灯

- 左下角指示灯一直闪烁。
- 默认模式下其他灯都关闭。
- 按一下Reset按钮，左上角和中间大灯（红色）闪烁一次
- bootloader模式下，左上角呼吸灯，中间大灯（绿色）常亮

## 运行行为

1. 通过`make flash`写入程序并退出bootloader模式后就开始运行
2. 程序运行结束后不会自动重启，需要手动按Reset按钮重启
3. `make term`仅仅用`pyterm`连接到板子查看输出，不影响程序运行
