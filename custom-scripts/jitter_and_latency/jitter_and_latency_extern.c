#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <rtdm/gpio.h>


RTIME usec = 1e3;
RTIME msec = 1e6;

int pin_out;
int pin_isr;


void pin_write(int pin, int value)
{
    int ret, error_code;
    
    ret = write(pin, &value, sizeof(value));
    if (ret == -1)
    {
        error_code = errno;
        rt_printf("Error: writing to led pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }
}


int pin_read(int pin)
{
    int ret, error_code, value;
    
    ret = read(pin, &value, sizeof(value));
    if (ret == -1)  
    {
        error_code = errno;
        rt_printf("Error: reading interrupt pin failed. Code %d, %s\n",
               error_code, strerror(error_code));
        exit(1);
    }

    return value;
}


void led_interrupt(void *arg)
{
    int ret, error_code, value = 1;
    int count = 0;
    RTIME time_prev = rt_timer_read();
    RTIME time_now;
    RT_TASK_INFO taskinfo;
    rt_task_inquire(NULL, &taskinfo);
    rt_printf("Executing task: %s\n", taskinfo.name);
    
    while (1)
    {
        pin_read(pin_isr);
        pin_write(pin_out, value);
        value = !value;
    }
}


void setup_gpio()
{
    int ret, error_code, value = 0;

    pin_out = open("/dev/rtdm/pinctrl-bcm2835/gpio2016", O_WRONLY);
    if (pin_out == -1)
    {
        error_code = errno;
        rt_printf("Error: opening led pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    ret = ioctl(pin_out, GPIO_RTIOC_DIR_OUT, &value);
    if (ret == -1)
    {
        error_code = errno;
        rt_printf("Error: led pin setup failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }
    
    ret = write(pin_out, &value, sizeof(value));
    if (ret == -1)
    {
        error_code = errno;
        rt_printf("Error: writing to led pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    pin_isr = open("/dev/rtdm/pinctrl-bcm2835/gpio2018", O_RDONLY);
    if (pin_isr == -1)
    {
        error_code = errno;
        rt_printf("Error: opening interrupt pin failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }

    int xeno_trigger = GPIO_TRIGGER_EDGE_RISING | GPIO_TRIGGER_EDGE_FALLING;
    ret = ioctl(pin_isr, GPIO_RTIOC_IRQEN, &xeno_trigger);
    if (ret == -1)
    {
        error_code = errno;
        rt_printf("Error: interrupt pin setup failed. Code %d, %s\n",
                error_code, strerror(error_code));
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int ret, error_code;
    RT_TASK task_interrupt;

    mlockall(MCL_CURRENT | MCL_FUTURE);

    setup_gpio();

    ret = rt_task_create(&task_interrupt, "Interrupt task", 0, 99, 0);
    if (ret != 0) 
    {
        error_code = -ret;
        rt_printf("Error: creating interrupt task failed. Code %d, %s\n", 
                error_code, strerror(error_code));
        exit(1);
    }

    ret = rt_task_start(&task_interrupt, &led_interrupt, 0);
    if (ret != 0) 
    {
        error_code = -ret;
        rt_printf("Error: starting interrupt task failed. Code %d, %s\n", 
                error_code, strerror(error_code));
        exit(1);
    }

    rt_printf("Press Ctrl + C to stop . . .");
    pause();
}
