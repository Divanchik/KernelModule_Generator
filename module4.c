#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("Generator device test");
MODULE_VERSION("0.01");

#define DEVICE_NAME "randgen"

// объявление функций символьного устройства
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;

// эта структура указывает на все функции нашего устройства
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

// когда процесс читает наше устройство, вызывается эта функция
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    int bytes_read = 0;
    while (len)
    {
        put_user(1, buffer++);
        len--;
        bytes_read++;
    }
    return bytes_read;
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
    if (device_open_count)
    {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

// вызывается, когда процесс закрывает наше устройство
static int device_release(struct inode *inode, struct file *file)
{
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init chrdev2_init(void)
{
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
