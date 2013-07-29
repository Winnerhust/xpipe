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
	assert(x.Bufsize() == fpathconf(fd[0],_PC_PIPE_BUF));
	x.Bufsize(20);
	//check point
	assert(x.Bufsize() == 20);
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
	pid_t pid = fork();
	if (pid == 0){
		x.receiveronly();

		childreq dd;
		x.recv((childreq *)&dd,sizeof(childreq));
		//check point
		assert(dd.recid == 10);
		assert(!strcmp(dd.billtype,"PAYBY"));
		exit(0);
	}
	else if (pid > 0){
		x.senderonly();

		childreq cc;
		cc.recid = 10;
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
	pid_t pid = fork();
	string item = "whose your daddy";
	if (pid == 0){//child process
		x.receiveronly();
		
		string rs;
		x.recv(rs);
		//check point
		assert(rs == item);
		exit(0);	
	}
	else if (pid > 0){//parent process
		int ret;
		x.senderonly();
		x.send(item);
		wait(&ret);
	}
}
/*test role*/
void test4()
{
	xpipe x;
	assert(x.role() == "sender and receiver");
	x.senderonly();
	assert(x.role() == "sender");
	x.receiveronly();
	assert(x.role() == "none");

	xpipe y;
	y.receiveronly();
	assert(y.role() == "receiver");

}
/*test read/recv*/
void test5()
{
	xpipe x;
	xpipe y;
	pid_t pid = fork();
	string x_item = "whose your daddy?";
	string y_item = "my father is Ligang!";
	if (pid == 0){//child process
		x.receiveronly();
		y.senderonly();

		string rs;
		x.recv(rs);
		//check point
		assert(rs == x_item);
		
		y.send(y_item);
		cout<<"child process:"<<y_item<<endl;
		exit(0);	
	}
	else if (pid > 0){//parent process
		int ret;
		x.senderonly();
		y.receiveronly();

		x.send(x_item);
		cout<<"parent process:"<<x_item<<endl;
		
		string ts;
		y.recv(ts);
		assert(ts == y_item);

		wait(&ret);
	}
}
int main(int argc, char const *argv[])
{
	test1();
	test2();
	test3();
	test4();
	test5();
	cout<<"pass all the tests"<<endl;
}
