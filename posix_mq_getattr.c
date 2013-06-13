/*************************************************************************
	> File Name: posix_mq_getattr.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 14时42分45秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

int main(int argc, char * argv[])
{
	mqd_t mqd;
	struct mq_attr attr;

	if (argc != 2) {
		printf("usage: mqgetattr <name>");
		exit(0);
	}
	mqd = mq_open(argv[1], O_RDONLY);
	mq_getattr(mqd, &attr);
	printf("max #msgs = %ld, max #bytes/msg = %ld, #currently on queue = %ld\n",
			attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

	mq_close(mqd);
	exit(0);
}
