/*************************************************************************
	> File Name: producer_consumer2.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月15日 星期六 18时59分23秒
 ************************************************************************/
//生产者和消费者之间的同步，消费者采用不断轮询的方式
//这种方式的缺点就是比较浪费资源
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems;  //条目，只读(无论生产者还是消费者)
/*
 *shared结构中的变量是共享的数据
 */
struct {
	pthread_mutex_t mutex;/*同步变量：互斥锁*/
	int buff[MAXNITEMS];
	int nput;/*nput是buff数组中下一次存放的元素下标*/
	int nval;/*nval是下一次存放的值*/
} shared = {
	PTHREAD_MUTEX_INITIALIZER /*对用于生产者线程之间同步的互斥锁做初始化*/
};
void *produce(void *);
void *consume(void *);

int main(int argc,char *argv[])
{
	/*
	 * 变量说明：tid_produce[]数组中保存每个线程的线程ID
	 * count[]是每个线程计数器
	 * tid_consume中保存单个的消费者的ID
	 */
	int i, nthreads, count[MAXNTHREADS];
	pthread_t tid_produce[MAXNTHREADS], tid_consume;
	/*命令行参数个数判断*/
	if (argc != 3) {
		printf("usage: producer_consumer1 <#iterms> <#threads>\n");
		exit(1);
	}
	/*
	 * argv[1]中指定生产者存放的条目数
	 * argv[2]中指定待创建的生产者线程的数目
	 */
	nitems = min(atoi(argv[1]), MAXNITEMS);
	nthreads = min(atoi(argv[2]), MAXNTHREADS);
   /*
	*set_concurrency函数用来告诉线程系统希望并发运行多少个线程
	*即就是设置并发级别
	*给并发级别加1，把额外的消费者线程也算在内
	*/
	set_concurrency(nthreads + 1);
	/*创建生产者线程：
	 *每个线程执行produce。
	 *tid_produce数组中保存每个线程的线程ID
	 *传递给每个生产者线程的参数是指向count数组中某个元素的指针
	 *首先吧计数器初始化为0，然后每个线程在每次往缓冲区中存放一
	 *个条目时给这个计数器加1.
	 *当一切生产完毕时，输出这个计数器数组中元素的值，以查看每个
	 *生产者线程分别存放了多少条目
	 */
	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}
	/*创建了生产者线程之后，立即创建消费者线程*/
	pthread_create(&tid_consume, NULL, consume, NULL);
	/*等待所有的生产者线程和消费者线程的终止(70~74)*/
	for (i = 0; i < nthreads; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);
	}
	pthread_join(tid_consume, NULL);
	return 0;
}
/*
 *创建生产者线程
 */
void *produce(void *arg) 
{
	for ( ; ; ) {
		pthread_mutex_lock(&shared.mutex);
		/*生产者的临界区中用来检测是否一切生产完毕的if判断语句
		 *需要注意的是：count元素的增加(通过arg指针)不属于该临
		 *界区，因为每个线程有各自的计数器
		 */
		if (shared.nput >= nitems) {
			pthread_mutex_unlock(&shared.mutex);
			return (NULL); //说明此时已经生产完毕
		}
		shared.buff[shared.nput] = shared.nval;
		shared.nput++;
		shared.nval++;
		pthread_mutex_unlock(&shared.mutex);
		*((int *) arg) += 1;
	}
}

void consume_wait(int i)
{
	/*
	 *consume_wait函数必须等待到生产者生产了第i个条目
	 *为了检查这种条件，必须要先给生产者的互斥锁上锁
	 *比较i和生产者的下标nput
	 *必须在查看nput下标之前获得互斥锁
	 *因为某个生产线程可能正处于该下标变量的更新中
	 */
	for ( ; ; ) {
		pthread_mutex_lock(&shared.mutex);
		if (i < shared.nput) {
			pthread_mutex_unlock(&shared.mutex);
			return;
		}
		pthread_mutex_unlock(&shared.mutex);
	}
}
void * consume(void *arg)
{
	int i;

	for (i = 0; i < nitems; i ++) {
		consume_wait(i);//从buff中取出下一个条目之前调用consume_wait等待生产者生产第i个条目
		if (shared.buff[i] != i)
			printf("buff[%d] = %d", i, shared.buff[i]);
	}
}












