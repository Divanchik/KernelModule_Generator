#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/fpu/api.h>
#include <linux/timex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dimadivan");
MODULE_DESCRIPTION("Pseudo-random number generator (LFSR A5 floats)");
MODULE_VERSION("0.01");

#define DEVICE_NAME "prng4"

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
static char buf[1024];

static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};


static unsigned l1, l2, l3;
#define l1_mask 0x7ffffU
#define l2_mask 0x3fffffU
#define l3_mask 0x7fffffU
unsigned _get(unsigned r, unsigned i) { return (r & (1 << i)) >> i; }
void _set1(unsigned *r, unsigned i) { *r = *r | (1 << i); }
void _set0(unsigned *r, unsigned i) { if (_get(*r, i)) *r = *r - (1 << i); }
unsigned f_l1(void) { return _get(l1, 18) ^ _get(l1, 17) ^ _get(l1, 16) ^ _get(l1, 13); }
unsigned f_l2(void) { return _get(l2, 21) ^ _get(l2, 20); }
unsigned f_l3(void) { return _get(l3, 22) ^ _get(l3, 21) ^ _get(l3, 20) ^ _get(l3, 7); }
unsigned tick(void)
{
    unsigned x = _get(l1, 8), y = _get(l2, 10), z = _get(l3, 10);
    unsigned F = (x & y) | (x & z) | (y & z);

    unsigned f = 0;
    unsigned res1 = 0;
    unsigned res2 = 0;
    unsigned res3 = 0;
    if (x == F)
    {
        res1 = _get(l1, 18);  // get result
        f = f_l1();           // calc feedback
        l1 <<= 1;             // shift
        if (f) _set1(&l1, 0); // write feedback
        l1 &= l1_mask;        // fix overflow
    }
    if (y == F)
    {
        res2 = _get(l2, 21);  // get result
        f = f_l2();           // calc feedback
        l2 <<= 1;             // shift
        if (f) _set1(&l2, 0); // write feedback
        l2 &= l2_mask;        // fix overflow
    }
    if (z == F)
    {
        res3 = _get(l3, 18);  // get result
        f = f_l3();           // calc feedback
        l3 <<= 1;             // shift
        if (f) _set1(&l3, 0); // write feedback
        l3 &= l3_mask;        // fix overflow
    }
    return res1 ^ res2 ^ res3;
}

union
{
    float fl[2];
}   numbuf;

union
{
    float fl;
    unsigned dw;
}   genbuf;

float absf(float x)
{
    return x < 0 ? -x : x;
}
//
float powf(float x, int n)
{
    float res;
    res = 1;
    while (n > 0)
    {
        res *= x;
        n--;
    }
    return res;
}

float logf(float x)
{
    int n;
    float x0, x1;
    n = 1;
    x0 = 0; x1 = 0;
    while (1)
    {
        if (n % 2 == 1)
            x1 += powf(x-1, n)/n;
        else
            x1 -= powf(x-1, n)/n;
        if (absf(x1-x0) < 0.000001)
            break;
        x0 = x1;
        n++;
    }
    return x1;
}

float sqrtf(float a)
{
    float x0;
    float x1;
    x0 = a/2;
    x1 = 0;
    while (1)
    {
        x1 = (a/x0 + x0)/2;
        if (absf(x1 - x0) < 0.000001)
            break;
        x0 = x1;
    }
    return x1;
}

void randfloat(void)
{
    int i;
    float res;
    genbuf.dw = 0x7FU << 23;
	  i=0;
    while (i < 23)
    {
        genbuf.dw |= (1 << i) * tick();
        i++;
    }
	  res = 1;
    i = 1;
    while (i <= 31)
    {
        res += powf(genbuf.fl - 1, i);
        while (res > 1)
            res -= 1.0;
        i++;
    }
    genbuf.fl = res;
}

void randfloat_normal(void)
{
    float f0, f1;
    float x, y, s;
    do
    {
        do
        {
            randfloat();
            x = tick() ? -genbuf.fl : genbuf.fl;
            randfloat();
            y = tick() ? -genbuf.fl : genbuf.fl;
            s = x*x + y*y;
        }
        while(s <=0 || s > 1);
        f0 = 1/2 + (x * sqrtf(-2 * logf(s) / s)) / 3;
        f1 = 1/2 + (y * sqrtf(-2 * logf(s) / s)) / 3;
    }
    while(f0 < 0 || f0 > 1 || f1 < 0 || f1 > 1);
    numbuf.fl[0] = f0;
    numbuf.fl[1] = f1;
}

// когда процесс читает наше устройство, вызывается эта функция
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    kernel_fpu_begin();
    int amount = 0;
    int counter = 0;
    unsigned long res;
    while (len)
    {
        int i=0;
        while(i<128)
        {
            randfloat_normal();
            memcpy(buf + i*8, &numbuf, 8);
            i++;
        }
        if (len > 1024)
            amount = 1024;
        else
            amount = len;
        res = copy_to_user(buffer, buf, amount);
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
    kernel_fpu_end();
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
    l1 = (unsigned)random_get_entropy();
    l2 = (unsigned)random_get_entropy();
    l3 = (unsigned)random_get_entropy();
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
