- coap是一个基于UDP的应用层协议，类似HTTP，默认端口为`5683`
- RIOT的coap实现非常难用
- 只要使用了模块`gcoap`，就会自动创建一个线程，名称为`gcoap`，优先级为`6`，主线程优先级为`7`
- `gcoap`线程处理消息时，不管是接收、发送、请求、响应，都使用同一个`buffer`和`pdu`
- `gcoap`处理消息时不接收其他消息
- `gcoap`默认注册`/.well-known/core`，可以使用`GET`请求获得所有注册的`path`
- 服务端流程
  1. 创建一个`coap_resource_t`数组，称为`resources`
     - 每个`resource`包含`path`、`method`、`callback`
     - `method`是flag形式，可以多个`method`按位或，还可以表示匹配子路径
  2. 创建一个`gcoap_listener_t`结构体，称为`listener`
     - 包含`resources`、数组长度、传输类型（UDP或DTLS）
  3. 调用`gcoap_register_listener()`启动服务
     - 参数为`listener`
     - 注册是异步的，`callback`在线程`gcoap`上处理
     - 可以注册多个`listener`
- 客户端流程
  1. 创建一个`buffer`，大小为`CONFIG_GCOAP_PDU_BUF_SIZE`，默认`512`字节
  2. 创建一个`coap_pkt_t`变量，称为`pdu`
     - 这是一个用于填充`buffer`的辅助结构，包含指针、长度等信息
     - 后面的函数几乎都以它为参数，但实际写入的是`buffer`
  3. 使用`gcoap_req_init()`初始化`pdu`
     - 这一步建立`buffer`和`pdu`的联系
     - 这一步往`buffer`中写入`method`和`path`
     - `method`是enum形式，跟服务端`coap_resource_t`中用的**不一样**
     - `path`被拆解后以选项的格式写入，但服务端解析的时候不会被当作选项
  4. （可选）使用`coap_hdr_set_type()`设置消息类型
     - 如果设置为`COAP_TYPE_CON`，服务端的回复默认为`COAP_TYPE_ACK`
     - 如果不设置，默认为`COAP_TYPE_NON`，服务端的回复也是如此
     - 目前没发现这个标记有什么作用
  5. （可选）使用`coap_opt_add_xxx()`添加选项
     - 选项有专门的编码，一般一个选项占一个字节
     - 重复调用以添加多个选项
     - 选项必须严格按照定义顺序添加，否则线程崩溃（**建议不添加任何选项**）
  6. 使用`coap_opt_finish()`结束写入选项，返回当前已经写入`buffer`的字节长度
     - 如果使用参数`COAP_OPT_FINISH_PAYLOAD`，它会多写入一个尾部标记`0xff`，用于分隔header和payload
     - 如果使用参数`COAP_OPT_FINISH_NONE`，表示没有payload，它实际除了返回长度以外什么也不做
  7. （可选）写入正文数据
     - 此时`pdu.payload`指向`buffer`中尾部标记的后一个字节
     - 此时`pdu.payload_len`的值是`buffer`的剩余字节长度
     - 直接从`pdu.payload`开始写入数据即可，不需要更新`payload_len`，后面的函数不需要它
  8. 使用`gcoap_req_send()`发送消息
     - 参数包括`buffer`、**消息总长度**、`sock_udp_ep_t`（包含ip地址、端口）、`callback`、传输类型
     - 参数不包括`pdu`
     - 发送是异步的，`callback`在线程`gcoap`上处理
- 服务端回调函数
  1. （可选）如果`resource`开启了匹配子路径，可以使用`coap_get_uri_path()`从`pdu`获得完整`path`
  2. （可选）如果`resource`有多个`method`，可以使用`coap_get_method()`从`pdu`获得`method`
     - `method`是enum形式，跟客户端发送的数据一样
  3. 根据`pdu`的字段读取正文
     - 此时`pdu.payload`指向`buffer`中正文的第一个字节，跟客户端行为一致
     - 此时`pdu.payload_len`的值是正文长度，跟客户端行为**不**一致
  4. 使用`gcoap_resp_init()`初始化`pdu`
     - 这一步会重置`pdu`数据，用于发送消息
     - 会根据原`pdu`的消息类型设置新`pdu`的消息类型，但没啥用
     - 这一步往`buffer`中写入状态码`code`
     - 由于`gcoap`重用`buffer`，所以读取请求必须在这一步之前
  5. （可选）使用`coap_opt_add_xxx()`添加选项，选项的要求跟客户端的要求一致
  6. 使用`coap_opt_finish()`结束写入选项，参数、返回值、行为跟客户端一致
  7. （可选）写入正文数据
     - 此时`pdu.payload`和`pdu.payload_len`的含义跟客户端完全一致
     - 直接从`pdu.payload`开始写入数据即可，同样不需要更新`payload_len`
  8. 返回**消息总长度**即可，`gcoap`负责发回
     - 返回值只有消息总长度，`gcoap`从原`buffer`读取回复消息
- 客户端回调函数
  1. 根据参数`memo->state`查看响应状态，如果是`GCOAP_MEMO_RESP`说明收到了回复
  2. 根据`coap_get_code_xxx()`获得状态码`code`
  3. 根据`pdu`的字段读取正文
     - 此时`pdu.payload`指向`buffer`中正文的第一个字节，跟服务端行为一致
     - 此时`pdu.payload_len`的值是正文长度，跟服务端行为一致

coap报文格式，回复没有`path`

```
|<-    4   ->|<-  2  ->|                               |<-  1 ->|
|------------|---------|-------------------|-----------|--------|-----------|
|<- header ->|<-token->|<- path(encoded) ->|<-options->|<-mark->|<-payload->|

```

`header`格式，请求为`method`，回复为`code`

```
|<-  1 ->|<-     1     ->|<-   2  ->|
|--------|---------------|----------|
|<-type->|<-code/method->|<-  id  ->|
```
