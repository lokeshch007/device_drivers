#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>

dev_t dev = 0;
static struct class *dev_class;

static int __init char_driver_init(void)
{
	/*Allocating MAJOR,MINOR numbers dynamically */
	if(alloc_chrdev_region(&dev,0,1,"cha_dev_dynamic") < 0)
	{
		printk(KERN_INFO "Cannot allocate major,minor number for device\n");
		return -1;
	}

	printk(KERN_INFO "Major number:%d\tMinor number:%d\n",MAJOR(dev),MINOR(dev));

	/*Creating struct class*/
	if((dev_class=class_create(THIS_MODULE,"char_dev_class")) == NULL)
	{
		printk(KERN_INFO "Cannot create the struct class for device\n");
		goto r_class;
	}

	/*Creating device from class*/
	if((device_create(dev_class,NULL,dev,NULL,"char_dev"))==NULL)
	{
		printk(KERN_INFO "Cannot create device\n");
		goto r_device;
	}

	printk(KERN_INFO "char_dev inserted successfully\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;

}

void __exit char_driver_exit(void)
{
	device_destroy(dev_class,dev);/*delete device from /dev*/
	class_destroy(dev_class);/*delete class*/

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
