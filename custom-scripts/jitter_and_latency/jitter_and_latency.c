#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>
#include <rtdm/gpio.h>

#define NUM_SAMPLES 100000

RTIME usec = 1e3;

RTIME times_blink[NUM_SAMPLES];
RTIME times_interrupt[NUM_SAMPLES];

RT_SEM sem;

int pin22, pin24;
int value22 = 0;
int value24;


void blink_led(void *arg)
{
    int ret, i, error_code;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);

    rt_task_set_periodic(NULL, TM_NOW, 500 * usec);

    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        value22 = 1;
        times_blink[i] = rt_timer_read();
        ret = write(pin22, &value22, sizeof(value22));
        if (ret == -1)
        {
            error_code = errno;
            printf("Error: writing to led pin failed. Code %d, %s\n",
                    error_code, strerror(error_code));
            exit(1);
        }
        
        rt_task_sleep(250 * usec);
        value22 = 0;

        ret = write(pin22, &value22, sizeof(value22));
        if (ret == -1)
        {
            error_code = errno;
            printf("Error: writing to led pin failed. Code %d, %s\n",
                    error_code, strerror(error_code));
            exit(1);
        }

        rt_task_wait_period(NULL);
    }
}


void led_interrupt(void *arg)
{
    int ret, error_code, interrupt_count = 0;
    RTIME interrupt_time;

    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    
    while (interrupt_count < NUM_SAMPLES)
    {
        ret = read(pin24, &value24, sizeof(value24));
        interrupt_time = rt_timer_read();
        if (ret == -1)  
        {
            error_code = errno;
            printf("Error: reading interrupt pin failed. Code %d, %s\n",
                   error_code, strerror(error_code));
            exit(1);
        }

        times_interrupt[interrupt_count] = interrupt_time;
        interrupt_count++;
    }
    rt_sem_v(&sem);
}


RTIME* calc_time_diff(RTIME *times_blink, RTIME *times_interrupt, int num_of_samples)
{
    int i;
    RTIME *time_diff = (RTIME *)malloc(NUM_SAMPLES * sizeof(RTIME));
    for (i = 0; i < num_of_samples; ++i)
    {
        time_diff[i] = times_interrupt[i] - times_blink[i];
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


void create_time_diffs_csv(char * filename, RTIME number_of_values,
                  RTIME *time_values){
    RTIME n=0;
    FILE *file;
    file = fopen(filename,"w");
    while (n < number_of_values)
    {
       fprintf(file,"%u,%llu\n",n,time_values[n]);
       n++;
    } 
    fclose(file);
}


void setup_gpio()
{
    int ret, error_code;

    pin22 = open("/dev/rtdm/pinctrl-bcm2835/gpio2016", O_WRONLY);
    if (pin22 == -1)
    {
        error_code = errno;
        printf("Error: opening led pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    ret = ioctl(pin22, GPIO_RTIOC_DIR_OUT, &value22);
    if (ret == -1)
    {
        error_code = errno;
        printf("Error: led pin setup failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }
    
    ret = write(pin22, &value22, sizeof(value22));
    if (ret == -1)
    {
        error_code = errno;
        printf("Error: writing to led pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    pin24 = open("/dev/rtdm/pinctrl-bcm2835/gpio2018", O_RDONLY);
    if (pin24 == -1)
    {
        error_code = errno;
        printf("Error: opening interrupt pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    int xeno_trigger = GPIO_TRIGGER_EDGE_RISING;
    ret = ioctl(pin24, GPIO_RTIOC_IRQEN, &xeno_trigger);
    if (ret == -1)
    {
        error_code = errno;
        printf("Error: interrupt pin setup failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int ret, error_code;
    RT_TASK task_led;
    RT_TASK task_interrupt;
    RTIME *time_diff;
    RTIME average;

    setup_gpio();

    ret = rt_sem_create(&sem, "sem1", 0, S_FIFO);
    if (ret != 0) 
    {
        error_code = -ret;
        printf("Error: code %d, %s\n", error_code, strerror(error_code));
        exit(1);
    }

    rt_task_create(&task_interrupt, "Interrupt task", 0, 99, 0);
    rt_task_start(&task_interrupt, &led_interrupt, 0);
    rt_task_create(&task_led, "Blink task", 0, 50, 0);
    rt_task_start(&task_led, &blink_led, 0);

    rt_sem_p(&sem, TM_INFINITE);

    printf("Calculating time differences...\n");
    time_diff = calc_time_diff(times_blink, times_interrupt, NUM_SAMPLES);
    printf("Writing time differentes to csv...\n");
    create_time_diffs_csv("/root/time_diff_xeno.csv", NUM_SAMPLES, time_diff);
    printf("Calculating average...\n");
    average = calc_average_time(NUM_SAMPLES, time_diff);
    free(time_diff);
    printf("Average: %llu\n", average);
}
