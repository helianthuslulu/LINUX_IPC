/*************************************************************************
	> File Name: pthread_rwlock_test1.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年07月06日 星期六 10时57分52秒
 ************************************************************************/
/*pthread_rwlock_test1.c说明：
 *使用读写锁实现四个线程读写一段程序的实例，共创建了四个新的线程，其中两个线程用来读取
 *数据，另外两个线程用来写入数据。在任意时刻，如果有一个线程在写数据，将阻塞所有其他线
 *程的任何操作
 * */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <bits/pthreadtypes.h>

#define WORK_SIZE 1024
static pthread_rwlock_t rwlock;

char work_area[WORK_SIZE];
int time_to_exit;

void *thread_to_read_o(void *arg);//读线程1
void *thread_to_read_t(void *arg);//读线程2
void *thread_to_write_o(void *arg);//写线程1
void *thread_to_write_t(void *arg);//写线程2

int main(int argc, char *argv[])
{
	int retval;
	//pthread_t用于声明线程ID
	pthread_t a_thread, b_thread, c_thread, d_thread;
	void *thread_result;

	retval = pthread_rwlock_init(&rwlock, NULL);
	if (retval != 0) {
		perror("rwlock initialization failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_create(&a_thread, NULL, thread_to_read_o, NULL);//创建一个新线程
	if (retval != 0) {
		perror("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_create(&b_thread, NULL, thread_to_read_t, NULL);
	if (retval != 0) {
		perror("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_create(&c_thread, NULL, thread_to_write_o, NULL);
	if (retval != 0) {
		perror("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_create(&d_thread, NULL, thread_to_write_t, NULL);
	if (retval != 0) {
		perror("thread create failed\n");
		exit(EXIT_FAILURE);
	}

	retval = pthread_join(a_thread, &thread_result);
	if (retval != 0) {
		perror("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_join(b_thread, &thread_result);
	if (retval != 0) {
		perror("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_join(c_thread, &thread_result);
	if (retval != 0) {
		perror("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	retval = pthread_join(d_thread, &thread_result);
	if (retval != 0) {
		perror("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	
	pthread_rwlock_destroy(&rwlock); //销毁读写锁
	exit(EXIT_FAILURE);
}

void *thread_to_read_o(void *arg)
{
	printf("thread read one try to get lock\n");

	pthread_rwlock_rdlock(&rwlock);//获取读取锁
	while (strncmp("end", work_area, 3) != 0) {
		printf("this is thread read one\n");
		printf("the characters is %s", work_area);
		pthread_rwlock_unlock(&rwlock);
		sleep(2);
		pthread_rwlock_rdlock(&rwlock);
		while (work_area[0] == '\0') {
			pthread_rwlock_unlock(&rwlock);
			sleep(2);
			pthread_rwlock_rdlock(&rwlock);
		}
	}
	pthread_rwlock_unlock(&rwlock);
	time_to_exit = 1;
	pthread_exit(0);
}

void *thread_to_read_t(void *arg)
{
	printf("thread read one try to get lock\n");

	pthread_rwlock_rdlock(&rwlock);
	while (strncmp("end", work_area, 3) != 0) {
		printf("this is thread read two\n");
		printf("the characters is %s\n", work_area);
		pthread_rwlock_unlock(&rwlock);
		sleep(5);
		pthread_rwlock_rdlock(&rwlock);
		while (work_area[0] == '\0') {
			pthread_rwlock_unlock(&rwlock);
			sleep(5);
			pthread_rwlock_rdlock(&rwlock);
		}
	}
	pthread_rwlock_unlock(&rwlock);
	time_to_exit = 1;
	pthread_exit(0);
}

void *thread_to_write_o(void *arg)
{
	printf("this is write thread one try to get lock\n");
	while (!time_to_exit) {
		pthread_rwlock_wrlock(&rwlock);
		printf("this is write thread one\n. Input some text. Enter 'end' to finish\n");
		fgets(work_area, WORK_SIZE, stdin);
		pthread_rwlock_unlock(&rwlock);
		sleep(15);
	}
	pthread_rwlock_unlock(&rwlock);
	pthread_exit(0);
}

void *thread_to_write_t(void *arg)
{
	sleep(10);
	while (!time_to_exit) {
		pthread_rwlock_wrlock(&rwlock);
		printf("this is write thread two\nInput some text.Enter 'end' to finish\n");
		fgets(work_area, WORK_SIZE, stdin);
		pthread_rwlock_unlock(&rwlock);
		sleep(20);
	}
	pthread_rwlock_unlock(&rwlock);
	pthread_exit(0);
}
