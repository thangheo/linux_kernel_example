#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LED_ON 1
#define LED_OFF 0
#define LED_IOCTL_MAGIC 'k'
#define LED_IOCTL_ON _IO(LED_IOCTL_MAGIC, 0)
#define LED_IOCTL_OFF _IO(LED_IOCTL_MAGIC, 1)
#define LED_IOCTL_READ _IOR(LED_IOCTL_MAGIC, 2, int)

int main()
{
    int fd, val, ret;
    fd = open("/dev/led_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device...");
        return -1;
    }

    // turn on the LED
    ret = ioctl(fd, LED_IOCTL_ON);
    if (ret < 0) {
        perror("Failed to turn on the LED");
        return -1;
    }

    // read the LED status
    ret = ioctl(fd, LED_IOCTL_READ, &val);
    if (ret < 0) {
        perror("Failed to read the LED status");
        return -1;
    }
    printf("LED status: %d\n", val);

    // turn off the LED
    ret = ioctl(fd, LED_IOCTL_OFF);
    if (ret < 0) {
        perror("Failed to turn off the LED");
        return -1;
    }

    // read the LED status
    ret = ioctl(fd, LED_IOCTL_READ, &val);
    if (ret < 0) {
        perror("Failed to read the LED status");
        return -1;
    }
    printf("LED status: %d\n", val);

    close(fd);
    return 0;
}
