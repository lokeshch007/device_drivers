/** Character device driver with static allocated major and minor number **/

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>

/*Static allocation of fixed major number 235 with 0 as initial minor number*/
dev_t dev = MKDEV(235,0);

static int __init char_driver_init(void)
{
	/*Register character driver for 1 count with name*/
	register_chrdev_region(dev,1,"cha_dev_static");

	printk(KERN_INFO "Major number:%d\tMinor number:%d\n",MAJOR(dev),MINOR(dev));

	printk(KERN_INFO "char_dev inserted successfully\n");
	return 0;
}

void __exit char_driver_exit(void)
{
	/*unregistering device major and minor numbers*/
	unregister_chrdev_region(dev,1);

	printk(KERN_INFO "char_dev removed successfully\n");
	return;
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lokesh <lokeshhc007@gmail.com>");
MODULE_DESCRIPTION("Chracter driver with static major and minor numbers");
MODULE_VERSION("0.1");
