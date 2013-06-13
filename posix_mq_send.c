/*************************************************************************
	> File Name: posix_mq_send.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 19时43分45秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char *argv[]) 
{
	mqd_t mqd;
	void *ptr;
	size_t len;
	int prio;
	
	if (argc != 4) {
		printf("usage: mqsend <name> <#bytes> <priority>");
		exit(1);
	}
	len = atoi(argv[2]);
	prio = atoi(argv[3]);

	mqd = mq_open(argv[1], O_WRONLY);
	ptr = calloc(len, sizeof(char));
	mq_send(mqd, ptr, len, prio);

	exit(0);
}
