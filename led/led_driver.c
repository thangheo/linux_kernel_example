#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name Here");

#define LED_PIN 17
#define DEVICE_NAME "led"
#define CLASS_NAME "led_class"

static int major_number;
static struct class* led_class = NULL;
static struct device* led_device = NULL;

static int led_open(struct inode *inode, struct file *filp) {
    return 0;
}

static int led_release(struct inode *inode, struct file *filp) {
    return 0;
}

static ssize_t led_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    char status;
    status = gpio_get_value(LED_PIN) ? '1' : '0';
    if (copy_to_user(buf, &status, 1) != 0) {
        return -EFAULT;
    }
    return 1;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    char value;
    if (copy_from_user(&value, buf, 1) != 0) {
        return -EFAULT;
    }
    if (value == '1') {
        gpio_set_value(LED_PIN, 1);
    } else if (value == '0') {
        gpio_set_value(LED_PIN, 0);
    } else {
        return -EINVAL;
    }
    return 1;
}
static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int err = 0, value = 0;

    switch (cmd) {
        case LED_IOCTL_ON:
            gpio_set_value(LED_PIN, 1);
            break;
        case LED_IOCTL_OFF:
            gpio_set_value(LED_PIN, 0);
            break;
        case LED_IOCTL_READ:
            value = gpio_get_value(LED_PIN);
            err = copy_to_user((int *)arg, &value, sizeof(value));
            if (err != 0) {
                pr_err("Failed to copy LED status to user space\n");
                return -EFAULT;
            }
            break;
        default:
            pr_err("Invalid IOCTL command\n");
            return -EINVAL;
    }

    return 0;
}

static struct file_operations fops =
{
    .open = led_open,
    .release = led_release,
    .read = led_read,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
};

static int __init led_init(void) {
    int ret;
    printk(KERN_INFO "LED driver initializing\n");

    ret = gpio_request(LED_PIN, "led");
    if (ret < 0) {
        printk(KERN_ERR "Failed to request GPIO pin %d\n", LED_PIN);
        return ret;
    }

    ret = gpio_direction_output(LED_PIN, 0);
    if (ret < 0) {
        printk(KERN_ERR "Failed to set GPIO pin %d direction to output\n", LED_PIN);
        gpio_free(LED_PIN);
        return ret;
    }

    major_number = register_chrdev(0, DEVICE_NAME, &led_fops);
    if (major_number < 0) {
        printk(KERN_ERR "Failed to register character device\n");
        gpio_free(LED_PIN);
        return major_number;
    }

    led_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_class)) {
        printk(KERN_ERR "Failed to create device class\n");
        unregister_chrdev(major_number, DEVICE_NAME);
        gpio_free(LED_PIN);
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        printk(KERN_ERR "Failed to create device\n");
        class_destroy(led_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        gpio_free(LED_PIN);
        return PTR_ERR(led_device);
    }

    printk(KERN_INFO "LED driver initialized\n");
    return 0;
}
static void __exit led_exit(void) {
    printk(KERN_INFO "LED driver unloaded\n");

    // Turn off LED
    gpio_set_value(LED_GPIO_PIN, 0);

    // Release the GPIO pin
    gpio_free(LED_GPIO_PIN);

    // Destroy the device
    device_destroy(led_class, MKDEV(major_number, 0));
    class_unregister(led_class);
    class_destroy(led_class);
    unregister_chrdev(major_number, DEVICE_NAME);
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED driver for Raspberry Pi");