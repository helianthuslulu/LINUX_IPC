/*************************************************************************
	> File Name: posix_mq_sendII.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 21时12分26秒
 ************************************************************************/
//本程序是用于向消息队列中放置一个消息
//当所放的消息超出所设置的mq_maxmsg，那么
//程序执行就会阻塞，直到有另外的进程从该消息队列中取走消息之后
//本次写入才能成功返回
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>


int main(int argc, char *argv[])
{
	mqd_t mqd;
	int retval = 0;
	size_t len;
	unsigned int prio;
	char *ptr;

	if (argc != 4) {
		printf("usage:mq_send <name> <byte> <priority>");
		exit(1);
	}
	len = atoi(argv[2]);
	prio = atoi(argv[3]);
	/*
	 *以只写模式打开消息队列
	 */
	mqd = mq_open(argv[1], O_WRONLY);
	if (mqd == -1) {
		perror("打开消息队列失败");
		exit(1);
	}
	/*
	 * 动态申请一块内存空间
	 * */
	ptr = calloc(len, sizeof(char));
	if (ptr == NULL) {
		perror("申请空间失败");
		mq_close(mqd);
		exit(1);
	}
	retval = mq_send(mqd, ptr, len, prio);
	if (retval == -1) {
		perror("消息发送失败");
		mq_close(mqd);
		exit(1);
	}
	free(ptr);//释放内存
	return 0;
}
