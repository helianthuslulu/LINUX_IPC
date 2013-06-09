/*************************************************************************
	> File Name: posix_mq_unlink.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月07日 星期五 17时51分03秒
 ************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
	int retval;

	if (argc != 2) {
		printf("usage: mqunlink <name>");
		exit(0);
	}
	retval = mq_unlink(argv[1]);
	if (retval != 0) {
		fprintf(stderr,"unlink failed %d:%s\n", errno, (char *)strerror(errno));
		return -2;
	}
	return 0;
}
