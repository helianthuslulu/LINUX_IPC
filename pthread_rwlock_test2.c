/*************************************************************************
	> File Name: pthread_rwlock_test2.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年07月07日 星期日 15时46分41秒
 ************************************************************************/
//pthread_rwlock_test2.c:验证读写锁的默认顺序
//如果在main函数中用pthread_rwlock_wrlock上锁，那么
//如果所有线程都阻塞在写锁上的时候，优先处理的是被阻塞的写锁
//然后才处理读出锁
//如果在main函数中用pthread_rwlock_rdlock上锁，那么
//如果有读者正在读的时候即使后面到来的写者比另外一些到来的读者更早
//也是先处理完读者，才转而处理写者，这样会导致写饥饿
//
//由此(执行结果)可以看出，LINUX平台默认的是读者优先，如果想要以写者优先
//则需要做一些处理
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_rwlock_t rwlock;

void *readers(void *arg)
{
	pthread_rwlock_rdlock(&rwlock);
	printf("reader %d got the lock\n", (int)arg);
	pthread_rwlock_unlock(&rwlock);
	//return NULL;
} 
void *writers(void *arg)
{
	pthread_rwlock_wrlock(&rwlock);
	printf("writer %d got the lock\n", (int)arg);
	pthread_rwlock_unlock(&rwlock);
	//return NULL;
}

int main(int argc, char **argv)
{
	int retval, i;

	pthread_t writer_id, reader_id;
	pthread_attr_t attr;
	int nreadercount = 1, nwritercount = 1;

	if (argc != 2) {
		fprintf(stderr, "usage, <%s threadcount>", argv[0]);
		return -1;
	}
	retval = pthread_rwlock_init(&rwlock, NULL);
	if (retval) {
		fprintf(stderr, "init lock failed\n");
		return retval;
	}
	pthread_attr_init(&attr);
	//pthread_attr_setdetachstate用来设置线程的分离状态
	//也就是说一个线程怎么样终止自己，状态设置为PTHREAD_CREATE_DETACHED
	//表示以分离状态启动线程
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	//分别在main函数中对读出者和写入者加锁，得到的处理结果是不一样的
	pthread_rwlock_wrlock(&rwlock);
//	pthread_rwlock_rdlock(&rwlock);
	for (i = 0; i < atoi(argv[1]); i++) {
		if (random() % 2) {
			pthread_create(&reader_id, &attr, readers, (void *)nreadercount);
			printf("create reader %d\n", nreadercount++);
		} else {
			pthread_create(&writer_id, &attr, writers, (void *)nwritercount);
			printf("create writer %d\n", nwritercount++);
		}
	}
	pthread_rwlock_unlock(&rwlock);
	sleep(20);//sleep是为了等待另外的线程的执行
	return 0;	
}
