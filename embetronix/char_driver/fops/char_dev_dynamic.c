#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/cdev.h>

dev_t dev = 0;
static struct class *dev_class;
static struct cdev char_cdev;

static int __init char_driver_init(void);
static void __exit char_driver_exit(void);
static int char_open(struct inode *inode,struct file *file);
static int char_release(struct inode *inode,struct file *file);
static ssize_t char_read(struct file *file,char __user *buf,size_t len,loff_t *offset);
static ssize_t char_write(struct file *file,const char *buf,size_t len,loff_t *offset);

/*Initializing file operation structure*/
static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read			= char_read,
	.write		= char_write,
	.open			= char_open,
	.release	=	char_release,
};


static int char_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "char_open() called\n");
	return 0;
}

static int char_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "char_release() called\n");
	return 0;
}

static ssize_t char_read(struct file *file,char __user *buf,size_t len,loff_t *offset)
{
	printk(KERN_INFO "char_read() called\n");
	return 0;
}

static ssize_t char_write(struct file *file,const char *buf,size_t len,loff_t *offset)
{
	printk(KERN_INFO "char_write() called\n");
	return len;
}

static int __init char_driver_init(void)
{
	/*Allocating MAJOR,MINOR numbers dynamically */
	if(alloc_chrdev_region(&dev,0,1,"cha_dev_dynamic") < 0)
	{
		printk(KERN_INFO "Cannot allocate major,minor number for device\n");
		return -1;
	}

	printk(KERN_INFO "Major number:%d\tMinor number:%d\n",MAJOR(dev),MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&char_cdev,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&char_cdev,dev,1))<0)
	{
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_class;
	}

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
	cdev_del(&char_cdev);/*Deleting cdev structure*/
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
