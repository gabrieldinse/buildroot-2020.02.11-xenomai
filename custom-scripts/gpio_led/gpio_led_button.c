#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>

#define NTASKS 2


int pin22, pin23;
int value22 = 0;
int value23;

RT_TASK task[NTASKS];

void toggle_led_button(void *arg)
{
    int ret;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    printf("Executing task: %s\n", taskinfo.name);
    while(1)
    {
        ret = read(pin23, &value23, sizeof(value23));
        ret = write(pin22, &value22, sizeof(value22));
        printf("Toggle led value %s\n", (value22 == 0) ? "LOW" : "HIGH");
        value22 = !value22;
    }
}

void setup()
{
    int ret;
    pin22 = open("/dev/rtdm/pinctrl-bcm2835/gpio992", O_WRONLY);
    ret = ioctl(pin22, GPIO_RTIOC_DIR_OUT, &value22);
    pin23 = open("/dev/rtdm/pinctrl-bcm2835/gpio993", O_RDONLY);
    int xeno_trigger = GPIO_TRIGGER_EDGE_FALLING;
    ret = ioctl(pin23, GPIO_RTIOC_IRQEN, &xeno_trigger);
}


int main(int argc, char *argv[])
{
    char str[20];
    setup();
    sprintf(str, "Toggle led task");
    rt_task_create(&task[0], str, 0, 50, 0);
    rt_task_start(&task[0], &toggle_led_button, 0);

    printf("Press CTRL+C to end...\n");
    pause();
}
