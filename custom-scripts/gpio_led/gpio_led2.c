#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

#define NTASKS 3

RTIME sec = 1e9;
RTIME msec = 1e6;

int pin22, pin23, pin24;
int value22 = 0;
int value23, value24;

RT_TASK task[NTASKS];

void blink_led(void *arg)
{
    int ret;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    rt_task_set_periodic(NULL, TM_NOW, 500 * msec);
    while(1)
    {
        value22 = 1;
        ret = write(pin22, &value22, sizeof(value22));
        rt_task_sleep(250 * msec);
        value22 = 0;
        ret = write(pin22, &value22, sizeof(value22));
        rt_task_wait_period(NULL);
    }
}

void led_interrupt(void *arg)
{
    int ret, count = 0;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    
    while(1)
    {
        ret = read(pin24, &value24, sizeof(value24));
        count++;
        printf("Led blinked %d time%s\n", count, (count == 1) ? "" : "s");
    }
}

void setup()
{
    int ret;
    pin22 = open("/dev/rtdm/pinctrl-bcm2835/gpio992", O_WRONLY);
    ret = ioctl(pin22, GPIO_RTIOC_DIR_OUT, &value22);
    pin24 = open("/dev/rtdm/pinctrl-bcm2835/gpio994", O_RDONLY);
    int xeno_trigger = GPIO_TRIGGER_EDGE_RISING;
    ret = ioctl(pin24, GPIO_RTIOC_IRQEN, &xeno_trigger);
}


int main(int argc, char *argv[])
{
    char str[20];
    setup();
    sprintf(str, "Blink led task");
    rt_task_create(&task[0], str, 0, 50, 0);
    rt_task_start(&task[0], &blink_led, 0);
    sprintf(str, "Led interrupt task");
    rt_task_create(&task[1], str, 0, 99, 0);
    rt_task_start(&task[1], &led_interrupt, 0);

    printf("Press CTRL+C to end...\n");
    pause();
}
