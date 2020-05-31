/** Character device driver with dynamic allocated major and minor number **/

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>

dev_t dev = 0;

static int __init char_driver_init(void)
{
	/*Allocating MAJOR,MINOR numbers dynamically */
	if(alloc_chrdev_region(&dev,0,1,"cha_dev_dynamic") < 0)
	{
		printk(KERN_INFO "Cannot allocate major,minor number for device\n");
		return -1;
	}

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
MODULE_DESCRIPTION("Chracter driver with dynamic major and minor numbers");
MODULE_VERSION("0.1");
