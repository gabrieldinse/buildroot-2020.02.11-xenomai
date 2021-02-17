#include <stdio.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define NUM_SAMPLES 10000

RTIME usec = 1e3;

RTIME time_diff[NUM_SAMPLES - 1];
RTIME times[NUM_SAMPLES];

RT_TASK task;

RT_SEM sem;

void periodic_task(void *arg)
{
    int i; 
    rt_task_set_periodic(NULL, TM_NOW, 100 * usec);

    for (i = 0; i < NUM_SAMPLES; ++i)
    {
         times[i] = rt_timer_read();
         rt_task_wait_period(NULL);
    }
    rt_sem_v(&sem);
}

void calc_time_diff(RTIME *time_diff, RTIME *times, int num_of_samples)
{
    int i;
    for (i = 0; i < num_of_samples - 1; ++i)
    {
        time_diff[i] = times[i + 1] - times[i];
    }
}

RTIME calc_average_time(int num_of_samples, RTIME *time_diff)
{
    RTIME average = 0;
    int i;
    for (i = 0; i < num_of_samples; ++i)
    {
        average += time_diff[i];
    }
    return average / num_of_samples;
}

void create_time_diffs_csv(char * filename, unsigned int number_of_values,
                  RTIME *time_values){
    unsigned int n=0;
    FILE *file;
    file = fopen(filename,"w");
    while (n<number_of_values) {
       fprintf(file,"%u,%llu\n",n,time_values[n]);
       n++;
    } 
    fclose(file);
}

int main(int argc, char *argv[])
{
    RTIME average;
    rt_sem_create(&sem, "sem1", 0, S_FIFO);
    rt_task_create(&task, "Periodic", 0, 10, 0);
    rt_task_start(&task, &periodic_task, 0);
    printf("Starting task...\n");
    rt_sem_p(&sem, TM_INFINITE);

    printf("Calculating time differences...\n");
    calc_time_diff(time_diff, times, NUM_SAMPLES);
    printf("Writing time differeces to csv...\n");
    create_time_diffs_csv("/root/time_diff.csv", NUM_SAMPLES - 1, time_diff);
    printf("Calculating average...\n");
    average = calc_average_time(NUM_SAMPLES - 1, time_diff);
    printf("Average: %llu\n", average);
}
