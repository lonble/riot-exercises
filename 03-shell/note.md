- 有两种方法创建命令
  1. `shell_command_t`数组
     - `shell_command_t`是结构体，保存命令名称、描述、回调函数
     - **用空结构体来标识数组末尾**
     - 把数组传入命令处理函数
  2. `SHELL_COMMAND`
     - 这是个宏，创建一个结构体，然后使用gcc attribute把它放到指定内存区域
     - **它必须写在全局位置**
     - 命令处理函数的`commands`参数可以为`NULL`
     - 这是实验性功能，不建议使用
- `SHELL_DEFAULT_BUFSIZE`是`128`
- `STDIO_RX_BUFSIZE`是`64`
- `STDIO_RX_BUFSIZE`必须是2的整数幂，否则有BUG导致标准输入几乎不可用
- shell默认回显命令，可以通过定义宏`CONFIG_SHELL_NO_ECHO`为`1`关闭回显
- `shell_readline()`有BUG，当终端指针位于命令第一个字符时，会重置到行首
- 使用模块`shell_cmds_default`自动添加其他已使用模块的内置命令
- shell内置`help`命令，打印其他所有命令的名称和描述
- shell命令可以手动解析运行，灵活度高，可以处理命令返回值
  1. 使用`shell_readline()`获取输入，通过返回值处理EOF、缓冲区溢出问题。等待输入期间会阻塞线程
  2. 使用`shell_handle_input_line()`运行命令，函数返回值即是命令返回值
  3. 重复这些步骤以持续处理命令
- shell命令也可以自动运行，但无法处理返回值
  - `shell_run_once()`循环读取命令并执行，如果读到EOF则退出
  - `shell_run_forever()`循环调用`shell_run_once()`，永远不会退出
  - `shell_run()`是`shell_run_forever()`的别名
  - 因为`pyterm`不会把EOF传递给板子，所以这两个函数表现基本相同，都不会退出
- 
    
