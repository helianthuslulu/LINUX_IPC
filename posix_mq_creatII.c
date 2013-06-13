/*************************************************************************
	> File Name: posix_mq_creatII.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 16时36分39秒
 ************************************************************************/
//这个程序是对posix_mq_creat.c的一个改进版本 
//就是：这个程序允许指定所创建的队列的最大消息数
//和每个消息的最大size。但是不能只是指定其中一种
//而是两者(mq_maxmsg&mq_msgsize)都得指定
//需要注意的是，该程序的执行必须在超级用户权限之下进行
//否则无法创建新的消息队列
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	mqd_t mqd;
	int c, flags;
	struct mq_attr attr;
	int	FILE_MODE = S_IRUSR | S_IWUSR;

	flags = O_RDWR | O_CREAT;
	while ((c = getopt(argc, argv, "em:z:")) != -1) {
		switch (c) {
			case 'e':
				flags |= O_EXCL;
				break;
			case 'm':
				attr.mq_maxmsg = atol(optarg);
				break;
			case 'z':
				attr.mq_msgsize = atol(optarg);
				break;
		}
	}
	if (optind != argc - 1) {
		printf("usage: mq_creatII [-e] [-m maxmsg -z msgsize] <name>");
		exit(0);
	}
	if ((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) || (attr.mq_maxmsg == 0 && attr.mq_msgsize != 0)) {
		printf("must specify both -m maxmsg and -z msgsize");
		exit(0);
	}
	mqd = mq_open(argv[optind], flags, FILE_MODE, (attr.mq_maxmsg != 0) ? &attr : NULL);
	mq_close(mqd);
	exit(0);
}
