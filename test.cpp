
#include <iostream>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "xpipe.h"
using namespace std;


/*test Bufszie*/
void test1()
{
	xpipe x;
	int fd[2];
	pipe(fd);
	//check point
	assert(x.Bufsize()==fpathconf(fd[0],_PC_PIPE_BUF));
	x.Bufsize(20);
	//check point
	assert(x.Bufsize()==20);
}
/*test read/recv*/
/////////////////////////////////////
class childreq
{
public:
	long recid;
	char billtype[20];

};
void test2()
{
	xpipe x;
	pid_t pid=fork();
	if (pid==0)
	{
		x.DisWriteable();

		childreq dd;
		x.recv((childreq *)&dd,sizeof(childreq));
		//check point
		assert(dd.recid==10);
		assert(!strcmp(dd.billtype,"PAYBY"));
		exit(0);
	}
	else if (pid>0)
	{
		x.DisReadable();

		childreq cc;
		cc.recid=10;
		strcpy(cc.billtype,"PAYBY");

		x.send((childreq *)&cc,sizeof(childreq));
		int ret;
		wait(&ret);
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
		//check point
		assert(rs==item);
		exit(0);	
	}
	else if (pid>0)
	{
		int ret;
		x.DisReadable();
		x.send(item);
		wait(&ret);
	}
}
int main(int argc, char const *argv[])
{
	test1();
	test2();
	test3();
	cout<<"pass all the tests"<<endl;
}
