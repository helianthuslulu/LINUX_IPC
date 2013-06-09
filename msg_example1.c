/*************************************************************************
	> File Name: msg_example1.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年05月15日 星期三 21时43分37秒
 ************************************************************************/
//system V版本的消息队列，可以根据消息类型区分消息
//父子进程的话，用ID就可以区分。非父子进程的话就是服务端-客户端模型了，此时，
//必须通过一个交互建立起连接，利用SOCKET会比较简单，UNIX域套接字也可以

//来源：num7.iteye.com/blog/704694
//演示消息队列的创建 发送 读取 改变权限以及删除消息队列的各种操作
/*
 *msgrcv/msgsnd系统调用
 *功能描述：在消息队列上进行收发消息。为了发送消息，调用进程对消息队列必须有写权限
 *接受消息时必须有读权限。
 *用法：
 *#include <sys/types.h>
 *#include <sys/ipc.h>
 *#include <sys/msg.h>
 *int msgsnd(int msgid, const void *msgp, size_t msgsz, int msgflg);
 *size_t msgrcv(int msgid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
 *参数：
 *msgid:消息队列的识别码
 *msgp:指向消息缓冲区的指针，此位置用来暂时存储发送和接收的消息，是一个用户可以定义的通用结构，形态如下：
 *struct msgbuf {
 *	long mtype;  //消息类型，必须>0
 *	char mtext[1]；//消息文本
 * };
 *
 *msgsz:消息大小
 *msgtyp:从消息队列中读取的消息类型。如果值为零，则表示消息队列中的所有消息都会被读取
 *(mtype:这个是自己填写的，可以约定(如父进程用类型1，子进程用类型2)
 *当msgrcv的时候，参数msgtyp来指定要读取的消息类型，而对其他消息视而不见)
 *msgflg:用来指明核心程序在队列没有数据的情况下应采取的行动。
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_SEND_SIZE 80

struct mymsgbuf {
	long mtype;  //消息类型，必须>0
	char mtext[MAX_SEND_SIZE]; //消息文本
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text);
void read_message(int qid, struct mymsgbuf *qbuf, long type);
void remove_queue(int qid);
void change_queue_mode(int qid, char *mode);
void usage(void);

int main(int argc, char *argv[])
{
	key_t key;
	int msgqueue_id; //消息队列标识符
	struct mymsgbuf qbuf; 

	if (argc == 1)
		usage();
	/*create unique key via call to ftok()*/
	key = ftok(".", 'm');
	
	/*open the queue - create if necessary*/
	if ((msgqueue_id = msgget(key, IPC_CREAT | 0660)) == -1) {
		perror("msgget");
		exit(1);
	}

/*关于argv[1][0]的解释来自：stackoverflow.com/questions/11757951/what-is-the-meaning-of-argv10(foreign Web)
 * as others have pointed out, argv[1] is the second string in the string array argv,
 * and strings are character arrays so argv[1][0] is the first character in the second
 * string.
 * that is(也就是说)：it means the first character of the second string of argv.
 * */
	switch (tolower(argv[1][0])) {
		case 's': send_message(msgqueue_id, (struct mymsgbuf *)&qbuf, atol(argv[2]), argv[3]);
				  break;
		case 'r': read_message(msgqueue_id, &qbuf, atol(argv[2])); //atol:将字符串转换成长整型数
				  break;
		case 'd': remove_queue(msgqueue_id);
				  break;
		case 'm': change_queue_mode(msgqueue_id, argv[2]);
				  break;
		default: usage();
	}
	return 0;
}

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
	/*send a message to the queue*/
	printf("sending a message ...\n");
	qbuf->mtype = type;
	strcpy(qbuf->mtext, text);

	if ((msgsnd(qid, (struct mymsgbuf *)qbuf, strlen(qbuf->mtext) + 1, 0)) == -1) { 
		//msgsnd()最后一个参数为0表示调用进程挂起执行，直到达到系统所规定的最大值为止，并发送消息
		perror("msgsnd");
		exit(1);
	}
}

void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
	/*read a message from the queue*/
	printf("reading a message...\n");
	qbuf->mtype = type;
	msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
	printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}

void remove_queue(int qid)
{
	/*remove the queue*/
	msgctl(qid, IPC_RMID, 0); 
}

void change_queue_mode(int qid, char *mode)
{
	struct msqid_ds myqueue_ds;  //描述整个消息队列的基本属性的数据结构
	/*get current info*/
	msgctl(qid, IPC_STAT, &myqueue_ds); //获取ipc_perm(权限)参数，并将其存放在myqueue_ds所指的结构中
	/*convert and load the mode*/
	sscanf(mode, "%ho", &myqueue_ds.msg_perm.mode);
	/*update the mode*/
	msgctl(qid, IPC_SET, &myqueue_ds);
}

void usage(void)
{
	fprintf(stderr, "msgtool - A utility for tinkering with msg queuesn");
	fprintf(stderr, "nUSAGE:msgtool (s)end n");
	fprintf(stderr, "(r)ecv n");
	fprintf(stderr, "(d)eleten");
	fprintf(stderr, "(m)ode n");
	exit(1);
}
