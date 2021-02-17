#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10

static RT_TASK  t1;
static RT_TASK  t2;
static RT_TASK  t3;
static RT_TASK  t4;


RT_SEM sem1;
RT_SEM sem2;

int global = 0;

void taskOne(void *arg)
{
    int i;
    for (i=0; i < ITER; i++) {
        rt_sem_p(&sem2, TM_INFINITE);
        printf("I am taskOne and global = %d................\n", ++global);
        rt_sem_v(&sem1);
    }
}

void taskTwo(void *arg)
{
    int i;
    for (i=0; i < ITER; i++) {
        rt_sem_p(&sem1, TM_INFINITE);
        printf("I am taskTwo and global = %d----------------\n", --global);
        rt_sem_v(&sem2);
    }
}

int main(int argc, char* argv[]) {
    rt_sem_create(&sem1, "semaphore1", 0, S_FIFO);
    rt_sem_create(&sem2, "semaphore2", 1, S_FIFO);
    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    rt_task_create(&t3, "task3", 0, 1, 0);
    rt_task_create(&t4, "task4", 0, 1, 0);
    rt_task_start(&t3, &taskOne, 0);
    rt_task_start(&t4, &taskTwo, 0);
    return 0;
} 
