/*************************************************************************
	> File Name: posix_msgqueue_recive.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月09日 星期日 17时17分05秒
 ************************************************************************/
//这个程序是从一个消息队列中读取某个消息
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int c,flags;
	mqd_t mqd;
	ssize_t n;
	unsigned int prio;
	void *buf;
	struct mq_attr attr;
	flags = O_RDONLY;

	while ((c = getopt(argc, argv, "n")) !=-1) {
		switch(c) {
			case 'n':
				flags |= O_NONBLOCK;
				break;
		}
	}
	if (optind != argc - 1) {
		printf("usage:mqreceive: [-n] <name>\n");
		exit(0);
	}
	mqd = mq_open(argv[optind], flags);
	/*
	 *调用mq-getattr打开队列并取得属性
	 *需要通过该系统调用确定最大消息的大小，
	 *因为必须为调用mq_receive分配一个这样大小
	 *的缓冲区。最后输出所读消息的大小以及属性
       	*/
	mq_getattr(mqd, &attr);
	buf = malloc(attr.mq_msgsize);

	n = mq_receive(mqd, (char *)buf, attr.mq_msgsize, &prio);
	printf("read %ld bytes, priority = %u\n", (long)n, prio);
	exit(0);
}
