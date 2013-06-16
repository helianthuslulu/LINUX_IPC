/*************************************************************************
	> File Name: producer_consumer3.c
	> Author: helianthus_lu
	> mail: helianthus.lu@gmail.com 
	> Created Time: 2013年06月15日 星期六 21时18分52秒
 ************************************************************************/
//在单生产者-多消费者模型中利用调价变量来等待和发送信号
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems;  //条目，对生产者和消费者来说都是只读的
int buff[MAXNITEMS];
/*
 *把生产者变量和互斥锁收集到一个结构put中
 *生产者使用这个结构
 */
struct {
	pthread_mutex_t mutex;
	int nput;
	int nval;
} put = {
	PTHREAD_MUTEX_INITIALIZER
};
/*
 *把计数器、条件变量和互斥锁收集到一个结构中
 *下一个结构含有一个计数器、一个条件变量和一个互斥锁
 *把条件变量初始化为PTHREAD_MUTEX_INITIALIZER
 */
struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int nready;
} nready = {
	PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
};

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
void *produce(void *arg)
{
	for ( ; ; ) {
		//当生产者往数组buff中放置了一个新条目时，用put.mutex来为临界区上锁(92~100)
		pthread_mutex_lock(&put.mutex);
		if (put.nput >= nitems) {
			pthread_mutex_unlock(&put.mutex);
			return (NULL);
		}
		buff[put.nput] = put.nput;
		put.nput++;
		put.nval++;
		pthread_mutex_unlock(&put.mutex);
	
		//通知消费者
		//用来统计准备好由消费者处理的条目数的计数器nready.nready加1
		//在加1之前，如果该计数器的值为0，就调用pthread_cond_signal
		//唤醒可能正在等待其值变为任意非零数的任意线程(如消费者)
		//由此可以看出与该计数器关联的互斥锁和条件变量的相互作用
		//该计数器是在生产者和消费者之间共享的，因此
		//只有锁住与之关联的互斥锁时才能访问它
		//而与之关联的条件变量则用来等待和发送信号
		pthread_mutex_lock(&nready.mutex);
		if (nready.nready == 0)
			pthread_cond_signal(&nready.cond);
		nready.nready++;
		pthread_mutex_unlock(&nready.mutex);

		*((int *) arg) += 1; //即conut加1
	}
}
void *consume(void *arg)
{
	int i;

	for (i = 0; i < nitems; i++) {
		//消费者等待nready.nready变为非0
		//既然这个计数器实在所有的生产者和消费者之间
		//共享的，那么只有锁住它，才能测试它的值
		pthread_mutex_lock(&nready.mutex);
		while (nready.nready == 0)
			//phtread_cond_wait原子的执行如下两个动作：
			//(1)给互斥锁nready.mutex解锁
			//(2)把调用线程投入睡眠直到另外的线程就该条件变量而
			//调用pthread_cond_signal时才能唤醒
			pthread_cond_wait(&nready.cond, &nready.mutex);
		nready.nready--;
		pthread_mutex_unlock(&nready.mutex);

		if (buff[i] != i)
			printf("buff[%d] = %d\n", i, buff[i]);
	} 
}
