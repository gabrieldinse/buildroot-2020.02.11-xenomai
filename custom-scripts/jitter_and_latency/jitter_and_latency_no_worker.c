#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>
#include <rtdm/gpio.h>

#define NUM_SAMPLES 10000

RTIME usec = 1e3;
RTIME sec = 1e9;
RTIME msec = 1e6;

RTIME times[NUM_SAMPLES];
RTIME times2[NUM_SAMPLES];

RT_SEM sem;

#define NTASKS 3

int pin22, pin23, pin24;
int value22 = 0;
int value23, value24;

RT_TASK task[NTASKS];

void blink_led(void *arg)
{
    int ret, i;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    rt_task_set_periodic(NULL, TM_NOW, 500 * usec);

    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        value22 = 1;
        times[i] = rt_timer_read();
        ret = write(pin22, &value22, sizeof(value22));
        rt_task_sleep(250 * usec);
        value22 = 0;
        ret = write(pin22, &value22, sizeof(value22));
        rt_task_wait_period(NULL);
    }
}

void led_interrupt(void *arg)
{
    int ret, i;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        ret = read(pin24, &value24, sizeof(value24));
        times2[i] = rt_timer_read();
    }
    rt_sem_v(&sem);
}

RTIME* calc_time_diff(RTIME *times, RTIME *times2, int num_of_samples)
{
    int i;
    RTIME *time_diff = (RTIME *)malloc(NUM_SAMPLES * sizeof(RTIME));
    for (i = 0; i < num_of_samples; ++i)
    {
        time_diff[i] = times2[i] - times[i];
    }
    return time_diff;
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

void doWork(void *arg) {
  printf("Start working \n");
  while (1) {
    rt_task_sleep(200*usec);
    printf(".");  
    rt_timer_spin(500*usec); 
  }
}

void setup()
{
    int ret;
    pin22 = open("/dev/rtdm/pinctrl-bcm2835/gpio992", O_WRONLY);
    ret = ioctl(pin22, GPIO_RTIOC_DIR_OUT, &value22);
    ret = write(pin22, &value22, sizeof(value22));
    pin24 = open("/dev/rtdm/pinctrl-bcm2835/gpio994", O_RDONLY);
    int xeno_trigger = GPIO_TRIGGER_EDGE_RISING;
    ret = ioctl(pin24, GPIO_RTIOC_IRQEN, &xeno_trigger);
}

int main(int argc, char *argv[])
{
    char str[20];
    RTIME *time_diff;
    RTIME average;
    setup();

    rt_sem_create(&sem, "sem1", 0, S_FIFO);
    rt_task_create(&task[1], "Interrupt task", 0, 99, 0);
    rt_task_start(&task[1], &led_interrupt, 0);
    rt_task_create(&task[0], "Blink task", 0, 50, 0);
    rt_task_start(&task[0], &blink_led, 0);
//    rt_task_create(&task[2], "doWork task", 0, 49, 0);
//    rt_task_start(&task[2], &doWork, 0);

    rt_sem_p(&sem, TM_INFINITE);

    printf("Calculating time differences...\n");
    time_diff = calc_time_diff(times, times2, NUM_SAMPLES);
    printf("Calculating average...\n");
    average = calc_average_time(NUM_SAMPLES, time_diff);
    printf("Average: %llu\n", average);

}
