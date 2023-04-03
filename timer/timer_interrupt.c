#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

// Define the timer interrupt handler function
static irqreturn_t timer_interrupt_handler(int irq, void *dev_id)
{
    printk(KERN_ALERT "Timer interrupt occurred!\n");
    return IRQ_HANDLED;
}

// Define the module initialization function
static int __init timer_interrupt_init(void)
{
    int irq = 0; // Timer interrupt is IRQ0 on x86 systems

    // Register the timer interrupt handler function
    if (request_irq(irq, timer_interrupt_handler, IRQF_TIMER, "timer_interrupt", NULL) != 0) {
        printk(KERN_ALERT "Failed to register timer interrupt handler\n");
        return -EBUSY;
    }

    printk(KERN_ALERT "Timer interrupt handler registered\n");

    return 0;
}

// Define the module cleanup function
static void __exit timer_interrupt_exit(void)
{
    int irq = 0; // Timer interrupt is IRQ0 on x86 systems

    // Unregister the timer interrupt handler function
    free_irq(irq, NULL);

    printk(KERN_ALERT "Timer interrupt handler unregistered\n");
}

module_init(timer_interrupt_init);
module_exit(timer_interrupt_exit);
