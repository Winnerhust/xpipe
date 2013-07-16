xpipe
=====
**无名管道的C++封装类**，用于父子进程进行通信
基础介绍
-----



unix下一切皆文件，管道也不例外。无名管道pipe定义在<unistd.h>中。
```c++
	#include <unistd.h>
	int pipe(int fd[2]);
```
其中fd[0]是读端，fd[1]是写端，fd[1]的输出是fd[0]的输入,因此管道是一个有向的半双工通信方式。使用`write(fd[1],...)`和`read(fd[0],...)`对管道中的信息进行读写。无名管道通常运用于父子进程间通信。关闭读端或者写端是使用`close`函数，同文件句柄一样，关闭后不能重新打开。如果关闭后使用该端，系统会发送一个`SIGPIPE`的信号。作为一个文件，管道有一个缓存大小限制，这是一个运行时限制，使用`fpathconf`函数可以查看其大小，类型名为`_PC_PIPE_BUF`.
如:
```c++
	cout<<fpathconf(fd[0],_PC_PIPE_BUF)<<endl;
```
在我的 `Ubuntu10.10` 下为`4096`字节，刚好一页大小。而在109的`AIX`服务器上，管道大小的限制则为`32768`字节。

读写管道使用系统函数read和write,如：
```c++
	write(m_fd[1],content.c_str(),content.length());
```
这能体现管道作为文件的本质，但不能体现通信的意图，因此我将管道的读写封装为与socket中发送和接收。
```c++
	ssize_t xpipe::send(void *buf, size_t n)
	{
		return write(m_fd[1], buf, n);
	}
	ssize_t xpipe::recv(void *buf, size_t nbytes)
	{
		return read(m_fd[0], buf, nbytes);
	}
```
使用中，通信的内容常常为字符串，上述两个函数不仅能满足这个要求，还能传递一些简单结构体消息（稍后在讨论），但是每次都要输入长度。为简化开发，我将`send`和`recv`重载，作为特化方法，方便字符串的传递。使用方法非常简单，如：
```c++
	xpipe x;
	x.send("Whose your daddy?");
	string rs;
	x.recv(rs);
```
关于简单结构体，需要作个说明，这里指的是由C++基本类型组合而成的结构体，如：
```c++
	class child
	{
	public:
		long id;
		char name[20];
	};
```
注意: **string不是基本类型** 。传递结构体消息示例如下：
```c++
	xpipe x;
	child cc;
	cc.id=10;
	strcpy(cc.name,"PAYBY");
	x.send((child *)&cc,sizeof(child));
	/*-------------------------*/
	child dd;
	x.recv((child *)&dd,sizeof(child));	
```
通信设计
----
文件是常见的通信媒介。但对文件的读写必须要加上读写的角色信息才能体现通信的过程。一个简单的单向通信包含消息发送方和消息接收方。对管道读写时常常有可以看到接收进程（读进程）关闭写端口，发送进程（写进程）关闭读端口，这样做是为了确保信息流向的单一，以免信息从接收进程流向发送进程。对通信而言，这依然不够直观。单向的信息流动中，一方仅仅是发送者（`senderonly`）,另一方仅仅是接收者（`receiveronly`）。因此，在父子进程通信过程中，给他们指定角色就可以了。
示例代码如下：
```c++
	xpipe x;
	pid_t pid=fork();
	string item="whose your daddy";
	if (pid==0)
	{//child process
		x.receiveronly();
		
		string rs;
		x.recv(rs);
		//check point
		assert(rs==item);
		exit(0);	
	}
	else if (pid>0)
	{//parent process
		int ret;
		x.senderonly();
		x.send(item);
		wait(&ret);
	}
```
在示例代码中父进程被指定为发送者（`x.senderonly();`），不能通过`x`管道进行接收信息，子进程被指定为接收者(`x.receiveronly();`)，不能通过`x`管道进行发送信息。要实现父子进程的互相通信。可以在指定另一个管道，将子进程指定为发送者，父进程指定为接收者。（见使用示例）

使用示例
----
父子进程互相通信
```c++
	xpipe x;
	xpipe y;
	pid_t pid=fork();
	string x_item="whose your daddy?";
	string y_item="my father is Ligang!";
	if (pid==0)
	{//child process
		x.receiveronly();
		y.senderonly();

		string rs;
		x.recv(rs);
		//check point
		assert(rs==x_item);
		
		y.send(y_item);
		cout<<"child process:"<<y_item<<endl;
		exit(0);	
	}
	else if (pid>0)
	{//parent process
		int ret;
		x.senderonly();
		y.receiveronly();

		x.send(x_item);
		cout<<"parent process:"<<x_item<<endl;
		
		string ts;
		y.recv(ts);
		assert(ts==y_item);

		wait(&ret);
	}
```
预期结果为：
```
parent process:whose your daddy?
child process:my father is Ligang!
```

----
有问题，请[邮件联系我]<mailto://chenxueyou@163.com>
