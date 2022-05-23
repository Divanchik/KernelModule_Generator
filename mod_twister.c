#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("Pseudo-random number generator (Twister)");
MODULE_VERSION("0.01");

#define DEVICE_NAME "prng2"

// объявление функций символьного устройства
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
// static char onebuf[1024];
// static long rbuf[129];
static char rbuf[1024];
#define P 624
#define Q 397
#define MATRIX_A 0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL

static unsigned long mt[P];
static int mti = P + 1;

// эта структура указывает на все функции нашего устройства
static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

void init_genrand(unsigned long seed)
{
    mt[0] = seed & 0xffffffffUL;
    // for (mti=1;mti<P;mti++)
    mti = 1;
    while(mti < P)
    {
        mt[mti] = (1664525UL * mt[mti-1] + 1UL);
        mt[mti] &= 0xffffffffUL;
        mti++;
    }
}

unsigned long genrand(void)
{
    unsigned long y;
    static unsigned long mag01[2] = {0x0UL, MATRIX_A};
    if (mti >= P)
    {
        int kk;
        if (mti == P + 1)
            init_genrand(5489UL);
        // for (kk = 0;kk < P - Q;kk++)
        kk = 0;
        while (kk < P - Q)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk+Q] ^ (y>>1) ^ mag01[y & 0x1UL];
            kk++;
        }
        // for (;kk < P - 1;kk++)
        while (kk < P - 1)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk+(Q-P)] ^ (y>>1) ^ mag01[y & 0x1UL];
            kk++;
        }
        y = (mt[P-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[P-1] = mt[Q-1] ^ (y>>1) ^ mag01[y & 0x1UL];
        mti = 0;
    }
    y = mt[mti++];
    // темперирование
    y ^= (y>>11);
    y ^= (y<<7) & 0x9d2c5680UL;
    y ^= (y<<15) & 0xefc60000UL;
    y ^= (y>>18);
    return y;
}

long genrand_31(void) { return (long)(genrand()>>1); }

// когда процесс читает наше устройство, вызывается эта функция
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    int amount = 0;
    int counter = 0;
    unsigned long res;
    while (len)
    {
        int i=0;
        while(i<256)
        {
            long tmp = genrand_31();
            memcpy(rbuf+i*4, &tmp, 4);
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
    init_genrand((unsigned long)major_num);
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
