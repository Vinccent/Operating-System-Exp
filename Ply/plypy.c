/**
 * Create a virtual char devices
 **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define MAXN 1024
#define PLYPY_DEV_NAME "plypy_chrdev"


/* static int plypy_dev_open(struct inode *, struct file *filp); */
static ssize_t plypy_dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t plypy_dev_write(struct file *, const char *, size_t, loff_t *);
/* static int plypy_dev_release(struct inode *, struct file *filp); */

struct file_operations fops =
{
    /* .open = plypy_dev_open, */
    /* .release = plypy_dev_release, */
    .read = plypy_dev_read,
    .write = plypy_dev_write
};

int Major;
struct plypy_pipe {
    wait_queue_head_t inq, outq;       /* read and write queues */
    char buffer[MAXN], *end;           /* static buffer */
    char *wp;                          /* where the data ends */
    struct semaphore sem;              /* mutual exclusion semaphore */
};

static struct plypy_pipe plypy_pipe;
static struct plypy_pipe *dev = &plypy_pipe;

static ssize_t plypy_dev_read(struct file *filp, char __user *buf, size_t count,
                              loff_t *offset)
{
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    /* There may be multiple readers, so the use of loop is necessary */
    while (dev->buffer == dev->wp) { /* nothing to read, wait for inputs */
        up(&dev->sem);

        if (wait_event_interruptible(dev->inq, (dev->buffer != dev->wp)))
            return -ERESTARTSYS;
        /* Loop and reacquire the lock */
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    /* read data */
    count = min(count, (size_t)(dev->wp - dev->buffer));
    if (copy_to_user(buf, dev->buffer, count)) {
        /* error happened */
        up(&dev->sem);
        return -EFAULT;
    }
    dev->wp = dev->buffer;
    up(&dev->sem);

    wake_up_interruptible(&dev->outq);
    return count;
}


static ssize_t plypy_dev_write(struct file *filp, const char __user *buf,
                               size_t count, loff_t *offset)
{
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    while (dev->buffer != dev->wp) { /* the old data haven't been retrieved */
        up(&dev->sem);
        if (wait_event_interruptible(dev->outq, (dev->buffer == dev->wp)))
            return -ERESTARTSYS;
        /* P and loop again */
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    count = min(count, (size_t)( dev->end - dev->buffer ));
    if (copy_from_user(dev->buffer, buf, count)) {
        /* error happened */
        up(&dev->sem);
        return -EFAULT;
    }
    dev->wp += count;
    up(&dev->sem);
    wake_up_interruptible(&dev->inq);

    return count;
}

static int plypy_init(void)
{
    plypy_pipe.end = dev->buffer+MAXN;
    plypy_pipe.wp = dev->buffer;
    init_waitqueue_head(&dev->inq);
    init_waitqueue_head(&dev->outq);
    sema_init(&dev->sem, 1);

    Major = register_chrdev(0, PLYPY_DEV_NAME, &fops);
    if (Major < 0) {
        return Major;
    }
    printk(KERN_INFO "The %s is assigned major number %d",
           PLYPY_DEV_NAME, Major);
    printk(KERN_INFO "Use 'mknod /dev/%s c %d 0' to create a file",
           PLYPY_DEV_NAME, Major);
    return 0;
}

static void plypy_exit(void)
{
    unregister_chrdev(Major, PLYPY_DEV_NAME);
    printk(KERN_INFO "The %s is destroyed", PLYPY_DEV_NAME);
}

module_init(plypy_init);
module_exit(plypy_exit);

MODULE_LICENSE("GPL");
