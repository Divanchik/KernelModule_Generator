#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("Character device test");
MODULE_VERSION("0.01");

#define DEVICE_NAME "one"

// объявление функций символьного устройства
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static char onebuf[1024];

// эта структура указывает на все функции нашего устройства
static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

// когда процесс читает наше устройство, вызывается эта функция
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    int amount = 0;
    while (len)
    {
        if (len > 1024)
            amount = 1024;
        else
            amount = len;
        unsigned long res;
        res = copy_to_user(buffer, onebuf, amount);
        if (res == 0)
            printk(KERN_INFO "Tried to copy %d bytes, %ld bytes failed.\n", amount, res);
        len -= amount;
    }
    return len;
}

// вызывается, когда процесс пытается записать что-то в наше устройство
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_ALERT "This operation is not supported.\n");
    return -EINVAL;
}

// вызывается, когда процесс открывает наше устройство
static int device_open(struct inode *inode, struct file *file)
{
    return 0;
}

// вызывается, когда процесс закрывает наше устройство
static int device_release(struct inode *inode, struct file *file)
{
    return 0;
}

static int __init chrdev2_init(void)
{
    int i = 0;
    while (i < 1024)
    {
        onebuf[i] = 1;
        i++;
    }
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_num < 0)
    {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    }
    else
    {
        printk(KERN_INFO "one module loaded with device major number %d\n", major_num);
        return 0;
    }
}

static void __exit chrdev2_exit(void)
{
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, World!\n");
}

// регистрируем функции модуля
module_init(chrdev2_init);
module_exit(chrdev2_exit);
