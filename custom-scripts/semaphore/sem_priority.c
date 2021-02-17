#include <stdio.h>
#include <alchemy/task.h>

#define NTASKS 3
RT_TASK task[NTASKS];

RTIME sec = 1e9;

void demo(void *arg)
{
    int num = *(int*)arg;
    rt_printf("RT: %d\n", num);
    RT_TASK_INFO curtaskinfo;
    rt_task_inquire(NULL,&curtaskinfo);

    rt_task_set_periodic(NULL, TM_NOW, num * sec );

    while(1)
    {
        rt_printf("Task %s executed\n", curtaskinfo.name);
        rt_task_wait_period(NULL);
    }
}

int main(int argc, char *argv[])
{
    char str[20];

    for (int i = 0; i < NTASKS; i++ )
    {
        int val = i + 1;
        // Task name cannot be repeated, so we create
        // a unique name for each task
        sprintf(str, "multi task %d", i);
        rt_task_create(task + i, str, 0, 50, 0);
        rt_task_start(task + i, &demo, &val);
    }
    
    printf("End program by CTRL-C\n");
    pause();
}
