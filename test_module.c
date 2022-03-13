#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("simple module example");
MODULE_VERSION("0.01");

static int __init test_module_init(void)
{
    printk(KERN_INFO "Hello world!\n");
    return 0;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Goodbye world!\n");
}

module_init(test_module_init);
module_exit(test_module_exit);