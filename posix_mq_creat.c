/*************************************************************************
	> File Name: posix_mq_creat.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月07日 星期五 15时52分01秒
 ************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int c, flags;
	mqd_t mqd;

	flags = O_CREAT | O_RDWR;
	while ((c = getopt(argc, argv, "e")) != -1) {
		switch (c) {
			case 'e': 
				flags |= O_EXCL;
				break;
		}
	}
	if (optind != argc - 1) {
		printf("usage: mqcreate [-e] <name>");
	}
	mqd = mq_open(argv[optind], flags, S_IRUSR | S_IWUSR, NULL);
	mq_close(mqd);
	exit(0);
}

