#ifndef __XPIPEH
#define __XPIPEH

#include <unistd.h>
#include <string>
#include <string.h>
#include <stdio.h>
using namespace std;
/*
	无名管道的C++封装类，用于父子进程进行通信
	作者 ：陈学友
	时间 ：2013年7月15日 20:30:58
	邮 箱：chen_xueyou@163.com
*/
class xpipe
{
public:
	xpipe();
	~xpipe();
///核心方法
	ssize_t send(void *buf, size_t n);
	ssize_t recv(void *buf, size_t nbytes);
///常用方法特化
	void 	send(const string &content);
	void 	recv(string &content);

//读写关闭操作
	void 	DisReadable();
	void 	DisWriteable();

//属性操作
	bool 	Readable(){return m_readable;}
	bool 	Writeable(){return m_writeable;}

	long 	Bufsize(long newbufsize=0);
	/* data */
private:
	int 	m_fd[2];
	bool 	m_readable;
	bool 	m_writeable;
	
	long 	m_bufsize;
	char *	m_buf;
};
#endif
