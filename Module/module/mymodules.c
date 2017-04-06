#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int mymodule_init(void) {
    printk("hello, my module was loaded!\n");
    return 0;
}

static void mymodule_exit(void) {
    printk("goodye, my module was unload!\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
