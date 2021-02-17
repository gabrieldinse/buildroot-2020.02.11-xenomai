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

void button_interrupt(void *arg)
{

}

void setup()
{
    int ret;
    pin22 = open("/dev/rtdm/pinctrl-bcm2835/gpio993", O_WRONLY);
    ret = ioctl(pin22, GPIO_RTIOC_DIR_OUT, &value22);
//    pin23 = open("/dev/rtdm/pinctrl-bcm2835/gpio23", O_RDONLY);
//    pin24 = open("/dev/rtdm/pinctrl-bcm2835/gpio23", O_RDONLY);
//    int xeno_trigger = GPIO_TRIGGER_EDGE_FALLING;
//    ret = ioctl(pin23, GPIO_RTIOC_IRQEN, &xeno_trigger);
//    xeno_trigger = GPIO_TRIGGER_EDGE_RISING;
//    ret = ioctl(pin24, GPIO_RTIOC_IRQEN, &xeno_trigger);
}


int main(int argc, char *argv[])
{
    char str[20];
    setup();
    sprintf(str, "Blink led task");
    rt_task_create(&task[0], str, 0, 50, 0);
    rt_task_start(&task[0], &blink_led, 0);
    printf("Press CTRL+C to end...\n");
    pause();
}
