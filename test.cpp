
#include <iostream>
#include <assert.h>
#include "xpipe.h"
using namespace std;

class childreq
{
public:
  long recid;
	char billtype[20];

};
/*test Bufszie*/
void test1()
{
	xpipe x;
	int fd[2];
	pipe(fd);
	assert(x.Bufsize()==fpathconf(fd[0],_PC_PIPE_BUF));
	x.Bufsize(20);
	assert(x.Bufsize()==20);
}
/*test read/recv*/
void test2()
{
	xpipe x;
	pid_t pid=fork();
	if (pid==0)
	{
		x.DisWriteable();

		childreq dd;
		x.recv((childreq *)&dd,sizeof(childreq));
		assert(dd.recid==10);
		assert(!strcmp(dd.billtype,"PAYBY"));
		cout<<"childreq dd:"<<dd.recid<<"  "<<dd.billtype<<endl;
	}
	else if (pid>0)
	{
		x.DisReadable();

		childreq cc;
		cc.recid=10;
		strcpy(cc.billtype,"PAYBY");

		x.send((childreq *)&cc,sizeof(childreq));
	}
}
/*test read/recv*/
void test3()
{
	xpipe x;
	pid_t pid=fork();
	string item="whose your daddy";
	if (pid==0)
	{
		x.DisWriteable();
		
		string rs;
		x.recv(rs);
		assert(rs==item);
		
	}
	else if (pid>0)
	{
		x.DisReadable();
		x.send(item);
	}
}
int main(int argc, char const *argv[])
{
	test1();
	test2();
	test3();
	cout<<"pass all the tests"<<endl;
}
