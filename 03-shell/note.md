1. `SHELL_COMMAND`是个宏，创建一个结构体，然后使用gcc attribute把它放到指定内存区域。**它必须写在全局位置**
2. `shell_run()`会阻塞当前线程
3. `SHELL_DEFAULT_BUFSIZE`是128
4. shell会回显命令
5. 命令提示符有BUG，当删除命令的第一个字符后，提示符也会被删除
