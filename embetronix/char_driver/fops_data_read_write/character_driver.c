#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/slab.h>/*for kmalloc()*/
#include<linux/uaccess.h>/*for copy_to.copy_from_user()*/
#include<linux/string.h>/*for memset() */

#define MEM_SIZE 1024
#define CLASS_NAME "lok_char_class"
#define DEVICE_NAME "lok_char_dev"


dev_t dev = 0;
static struct class *dev_class;
static struct cdev char_cdev;
static uint8_t kernel_buffer[MEM_SIZE];

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
	int read_data_size=0;
	printk(KERN_INFO "char_read() called\n");
	
	if(len > 1024)
	{
		printk(KERN_INFO "read size exceeded limit");
		return -1;
	}
	/*copying kernel buffer data to user buffer*/
	if((read_data_size=copy_to_user(buf,kernel_buffer,len)) == 0){
		printk(KERN_INFO "Data read done\n");
		return len;
	}
	else{
		printk(KERN_INFO "Unable to read %d bytes\n",read_data_size);
		return MEM_SIZE-read_data_size;
	}
}

static ssize_t char_write(struct file *file,const char *buf,size_t len,loff_t *offset)
{
	int write_data_size=0;
	printk(KERN_INFO "char_write() called\n");

	if(len > 1024)
	{
		printk(KERN_INFO "write size exceeded limit");
		return -1;
	}

	/*Copying user buffer data to kernel buffer*/
	if((write_data_size=copy_from_user(kernel_buffer,buf,len)) == 0){
		printk(KERN_INFO "Copying data from user buffer to kernel buffer donen");
		return len;
	}
	else{
		printk(KERN_INFO "Unable to write %d bytes\n",write_data_size);
		return MEM_SIZE-write_data_size;
	}
}

static int __init char_driver_init(void)
{
	/*Allocating MAJOR,MINOR numbers dynamically */
	if(alloc_chrdev_region(&dev,1,1,DEVICE_NAME) < 0)
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
	if((dev_class=class_create(THIS_MODULE,CLASS_NAME)) == NULL)
	{
		printk(KERN_INFO "Cannot create the struct class for device\n");
		goto r_class;
	}

	/*Creating device from class*/
	if((device_create(dev_class,NULL,dev,NULL,DEVICE_NAME))==NULL)
	{
		printk(KERN_INFO "Cannot create device\n");
		goto r_device;
	}

	printk(KERN_INFO "Kernel Buffer cleaned from previous date\n");
	printk(KERN_INFO "char_dev inserted successfully\n");

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

	/*Freeing kernel buffer*/
	kfree(kernel_buffer);
	printk(KERN_INFO "Kernel Buffer freed\n");
	
	printk(KERN_INFO "char_dev removed successfully\n");
	return;
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lokesh <lokeshhc007@gmail.com>");
MODULE_DESCRIPTION("Chracter driver with dynamic major and minor numbers");
MODULE_VERSION("0.1");
