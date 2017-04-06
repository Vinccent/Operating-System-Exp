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
#define MY_DEV_NAME "caesium_chrdev"


static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

struct file_operations fop = {
    //need ,
	.read = dev_read,
    //no . after the laste line
	.write = dev_write
};

struct pipe {
	wait_queue_head_t inq, outq; //read and write queues
	char buffer[MAXN], *end; //begin of buffer, and end of buffer;
	char *rp; //where to read and write;
	struct semaphore sem;
};

static struct pipe pipe;
static struct pipe *dev = &pipe;
int Major;

static int my_init(void) {
    pipe.end = dev->buffer + MAXN;
    pipe.rp = dev->buffer;
    init_waitqueue_head(&dev->inq);
    init_waitqueue_head(&dev->outq);
    sema_init(&dev->sem, 1);

    Major = register_chrdev(0, MY_DEV_NAME, &fop);
    if (Major < 0)
        return Major;
    printk(KERN_INFO "The %s assigned to major %d\n", MY_DEV_NAME, Major);
    return 0;
}

static void my_exit(void) {
    unregister_chrdev(Major, MY_DEV_NAME);
    printk(KERN_INFO "The %s is destroyed\n", MY_DEV_NAME);
}

static ssize_t dev_read(struct file *filp, char __user *buf, size_t count, loff_t *offset) {
	if (down_interruptible(&dev->sem)) 
        return -ERESTARTSYS;

    //nothing to read
    while (dev->buffer == dev->rp) {
        up(&dev->sem);

        if (wait_event_interruptible(dev->inq, (dev->rp != dev->buffer)))
            return -ERESTARTSYS;
        
        //loop, but first reacquire the lock
        if (down_interruptible(&dev->sem)) 
            return -ERESTARTSYS;
    }

    //have data to read
    if (dev->rp > dev->buffer) 
        count = min(count, (size_t)(dev->rp - dev->buffer));
    else /*wrapped*/
        count = min(count, (size_t)(dev->end - dev->buffer));
    //error occured
    if (copy_to_user(buf, dev->buffer, count)) {
        up(&dev->sem);
        return -EFAULT;
    }
    
    //clean buffer
    dev->rp = dev->buffer;
    up(&dev->sem);

    wake_up_interruptible(&dev->outq);
    return count;
}


static ssize_t dev_write(struct file *flip, const char __user *buf, size_t count, loff_t *offset) {
    if (down_interruptible(&dev->sem)) 
            return -ERESTARTSYS;

    //old data hasn't been read
    while (dev->buffer != dev->rp) {
        up(&dev->sem);

        if (wait_event_interruptible(dev->outq, (dev->buffer == dev->rp))) 
            return -ERESTARTSYS;

        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;

        }
        //to write
        count = min(count, (size_t)(dev->end - dev->buffer));
        //error occured;
        if (copy_from_user(dev->buffer, buf, count)) {
            up(&dev->sem);
            return -EFAULT;
        }

        dev->rp += count;
        up(&dev->sem);

        wake_up_interruptible(&dev->inq);
        return count;
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
