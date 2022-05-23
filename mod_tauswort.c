#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("Pseudo-random number generator (Tauswort)");
MODULE_VERSION("0.01");

#define DEVICE_NAME "prng1"

// объявление функций символьного устройства
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
// static char onebuf[1024];
// static long rbuf[129];
static char rbuf[1026];
static unsigned long s[5], b;

// эта структура указывает на все функции нашего устройства
static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

void init_taus(unsigned long seed)
{
	int i = 0;
	while (i < 5)
	{
		if (seed & 0xfffffff0UL)
		{
			s[i] = seed;
			i++;
		}
		seed = 1664525UL * seed + 1UL;
	}
}

long taus_31 (void)
{
	b = (((s[0] << 13) ^ s[0]) >> 19);
	s[0] = (((s[0] & 4294967294UL) << 12) ^ b);
	b = (((s[1] << 2) ^ s[1]) >> 25);
	s[1] = (((s[1] & 4294967288UL) << 4) ^ b);
	b = (((s[2] << 3) ^ s[2]) >> 11);
	s[2] = (((s[2] & 4294967280UL) << 17) ^ b);
	b = (((s[3] << 4) ^ s[3]) >> 15);
	s[3] = (((s[3] & 4294967270UL) << 10) ^ b);
	b = (((s[4] << 5) ^ s[4]) >> 16);
	s[4] = (((s[4] & 4294967280UL) << 13) ^ b);

	return (long) ((s[0] ^ s[1] ^ s[2] ^ s[3] ^ s[4]) >> 1);
}

// когда процесс читает наше устройство, вызывается эта функция
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    int amount = 0;
    int counter = 0;
    unsigned long res;
    while (len)
    {
        int i=0;
        while(i<171)
        {
            long tmp = taus_31();
            memcpy(rbuf+i*6, &tmp, 6);
            i++;
        }
        if (len > 1024)
            amount = 1024;
        else
            amount = len;
        res = copy_to_user(buffer, rbuf, amount);
        buffer += amount;
        if (res != 0)
        {
            printk(KERN_INFO "Tried to copy %d bytes, %ld bytes failed.\n", amount, res);
            amount -= res;
        }
        len -= amount;
        counter += amount;
        if (signal_pending(current))
            break;
        cond_resched();
    }
    return counter;
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
    device_open_count++;
    return 0;
}

// вызывается, когда процесс закрывает наше устройство
static int device_release(struct inode *inode, struct file *file)
{
    return 0;
}

static int __init chrdev2_init(void)
{
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    init_taus((unsigned long)major_num);
    if (major_num < 0)
    {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    }
    else
    {
        printk(KERN_INFO "Module3 loaded with device major number %d\n", major_num);
        return 0;
    }
}

static void __exit chrdev2_exit(void)
{
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Unloading module3, device was opened %d times.\n", device_open_count);
}

// регистрируем функции модуля
module_init(chrdev2_init);
module_exit(chrdev2_exit);
