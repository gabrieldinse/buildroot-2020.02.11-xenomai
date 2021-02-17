#include <stdio.h>
#include <alchemy/task.h>

#define NTASKS 5
RT_TASK task[NTASKS];

void demo(void *arg)
{
    int num = *(int*)arg;
    RT_TASK_INFO curtaskinfo;
    rt_task_inquire(NULL,&curtaskinfo);
    rt_printf("Task name: %s %d\n", curtaskinfo.name, num);
}

int main(int argc, char *argv[])
{
    char str[20];

    sprintf(str, "multi task");
    for (int i = 0; i < 5; i++ )
    {
        rt_task_create(task + i, str, 0, 1, 0);
        rt_task_start(task + i, &demo, &i);
    }
}
