/*************************************************************************
	> File Name: posix_mq_sysconf.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 22时32分49秒
 ************************************************************************/
//调用sysconf获取消息队列的限制
//sysconf函数用来获取运行时的配置信息的
//mq_maxmsg:队列中的最大消息数目
//mq_msgsize:给定消息的最大字节数
//消息队列的实现还定义了另外两个限制：
//MQ_OPEN_MAX:一个进程能够同时拥有的打开着消息队列的最大数目
//MQ_PRIO_MAX:任意消息的最大优先级加1
//事实上，可以从/proc/sys/fs/mqueue/中查看msg_max msgsize_max queues_max
//这些都是关于消息队列的限制
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("MQ_OPEN_MAX = %ld, MQ_PRIO_MAX = %ld\n",
			sysconf(_SC_MQ_OPEN_MAX), sysconf(_SC_MQ_PRIO_MAX));
	exit(0);
}
