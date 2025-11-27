1. RIOT的时钟从程序启动开始计时
2. LED灯在`main`函数返回后也不会熄灭，因为`main`函数返回实际上并不能终结程序
3. `ztimer_set`的本质是创建中断处理函数
4. 不要使用`ztimer_set`！！！全是BUG
