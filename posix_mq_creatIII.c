/*************************************************************************
	> File Name: posix_mq_creatIII.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月13日 星期四 20时43分23秒
 ************************************************************************/
//这个程序的功能还是创建一个消息队列
//只是实现的方式与前面2个版本(posix_mq_creat.c&posix_mq_creatII.c)
//稍有不同
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MQ_NAME ("/msgqueue")
#define MQ_FLAG (O_RDWR | O_CREAT | O_EXCL)
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main(int argc, char *argv[])
{
	mqd_t posix_mqd;
	int close_retval = 0, retval = 0;
	/*
	 函数说明：创建或者打开一个消息队列
	 返回值：调用成功时，返回消息队列的描述符，失败返回-1，错误原因存放在errno中
	 */
	posix_mqd = mq_open(MQ_NAME, MQ_FLAG, FILE_MODE, NULL);
	if (-1 == posix_mqd) {
		perror("mqueue创建失败\n");
		exit(EXIT_FAILURE);
	}
	close_retval = mq_close(posix_mqd);
	if (0 != close_retval) {
		perror("消息队列关闭失败");
		exit(1);
	}

#if 0
	/*
	 *函数说明：删除一个消息队列，就好比删除一个文件，其他进再也无法无法访问
	 *返回值：调用成功返回0，失败返回-1，错误原因存在errno中
	 */
	retval = unlink(posix_mqd);
	if (retval != 0) {
		perror("删除出错")；
		exit(1);
	}
	return 0;
#endif	
}

