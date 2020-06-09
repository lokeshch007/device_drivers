/* header files */
#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/sched.h>
#include<linux/ioctl.h>
#include<linux/proc_fs.h>

/************************ MACROS ***************************/
#define BUFFER_MAXLEN 4000 /* corresponds to 4k Memory page size */
#define WR_VALUE _IOW('a','a',int32_t*) /* IOCTL write value */
#define RD_VALUE _IOR('a','b',int32_t*) /* IOCTL read value */
/***********************************************************/

/************** global variable declarations ***************/
dev_t device_number;	/* Device major,minor number */
static struct cdev my_char_dev; 
static struct class *my_device_class_ptr;
static char mybuffer[BUFFER_MAXLEN];
int32_t value=0; /* IOCTL value */
char chl_array[20]="proc_array\n"; /* procfs data */
static int len = 1; /* for procfs */
/***********************************************************/

/***************Driver function declarations ***************/
static int chl_open(struct inode *inode_ptr,struct file *file_ptr);
static int chl_close(struct inode *inode_ptr,struct file *file_ptr);
static ssize_t chl_read(struct file *file_ptr,char __user *buffer,size_t length,loff_t *offset);
static ssize_t chl_write(struct file *file_ptr,const char __user *buffer,size_t length,loff_t *offset);
static loff_t chl_lseek(struct file *file_ptr,loff_t offset,int origin);
static long chl_ioctl(struct file *file, unsigned int cmd,unsigned long arg);
/***********************************************************/

/***************procfs function declarations ***************/
static int open_proc(struct inode *inode,struct file *file);
static int release_proc(struct inode *inode,struct file *file);
static ssize_t read_proc(struct file *filep,char __user *buffer,size_t length,loff_t *offset);
static ssize_t write_proc(struct file *filep,const char __user *buffer,size_t length,loff_t *offset);
/***********************************************************/

/*************** Driver function definitions****************/

/* Driver File operation structure initialization */
static struct file_operations chl_fops = {
	.owner		=	THIS_MODULE,
	.open			=	chl_open,
	.release	=	chl_close,
	.read			=	chl_read,
	.write		=	chl_write,
	.llseek		=	chl_lseek,
	.unlocked_ioctl	=	chl_ioctl
};

/* Driver open function */
static int chl_open(struct inode *inode_ptr,struct file *file_ptr)
{
	printk(KERN_INFO "CHL Driver's open(): PID of process using this device is %d\n",current->pid);
	return 0;
}

/* Driver release function */
static int chl_close(struct inode *inode_ptr,struct file *file_ptr)
{
	printk(KERN_INFO "CHL Driver's close()\n");
	return 0;
}

/* Driver read operation function */
static ssize_t chl_read(struct file *file_ptr,char __user *buffer,size_t length,loff_t *offset)
{
	/* variable declaration */
	int remaining_bytes_in_buffer=0;
	int bytes_to_read=0;
	int nbytes=0;
	remaining_bytes_in_buffer=BUFFER_MAXLEN-(*offset);

	/* if remaining bytes in buffer is more than length of bytes 
		 requested to read */
	if(remaining_bytes_in_buffer > length)
	{
		bytes_to_read=length;
	}
	else
	{
		bytes_to_read=remaining_bytes_in_buffer;
	}

	/* if bytes to be read are zero */
	if(!bytes_to_read)
	{
		printk(KERN_INFO "CHL Driver's read() : EOD (End of Device)\n");
		return -ENOSPC;
	}

	/* nbytes = no of bytes to read - no of bytes unread in buffer
	 * that means nbytes is no of bytes succesfully read */
	nbytes = bytes_to_read - 
		copy_to_user(buffer,mybuffer+*offset,bytes_to_read);

	/* updating offset to the startng index of unread bytes */
	*offset += nbytes;

	/* Return no of bytes successfullt read */
	return nbytes;
}

/* Driver write operation function */
static ssize_t chl_write(struct file *file_ptr,const char __user *buffer,size_t length,loff_t *offset)
{
	/* Variable declaration */
	int max_bytes_can_be_written=0;
	int bytes_to_write=0;
	int nbytes=0;

	max_bytes_can_be_written = BUFFER_MAXLEN - *offset;

	/* check that buffer has enough no of bytes to write
	 * requested length no of bytes */
	if(max_bytes_can_be_written > length)
	{
		bytes_to_write = length;
	}
	else
	{
		bytes_to_write = max_bytes_can_be_written;
	}

	/* if zero bytes to be written */
	if(!bytes_to_write)
	{
		printk(KERN_INFO "CHL Driver's write(): EOD(End of Device)\n");
		return -ENOSPC;
	}

	/* nbytes = no of bytes to be written - no of bytes not written
	 * that means no of bytes successfully written */
	nbytes = bytes_to_write - 
		copy_from_user(mybuffer+*offset,buffer,bytes_to_write);

	/* Move offset to the end point of bytes written in current call */
	*offset += nbytes;

	/* Return successfully written bytes */
	return nbytes;
}


/* lseek function for setting offset */
static loff_t chl_lseek(struct file *file_ptr,loff_t offset,int origin)
{
	/* Variable declaration */
	loff_t new_pos=0;

	/* Check the starting point from where offset to be updated 
	 * and update new position */
	switch(origin)
	{
		case 0	:	/* SEEK_SET :
								 set offset to given offse value*/
			new_pos = offset;
			break;

		case 1	:	/* SEEK_CUR : 
								 set offset to current position+offset value */
			new_pos = file_ptr->f_pos+offset;
			break;

		case	2	:	/* SEEK_END : 
								 set offset to end of file */
			new_pos = BUFFER_MAXLEN-offset;
			break;

		default	:	/* Error offset position */
			return -EINVAL;
	}

	if(new_pos > BUFFER_MAXLEN)
	{
		new_pos=BUFFER_MAXLEN;
	}
	else if(new_pos < 0)
	{
		new_pos = 0;
	}

	file_ptr->f_pos = new_pos;

	return new_pos;
}

/* IOCTL function */
static long chl_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
	switch(cmd){
		case WR_VALUE	:
			copy_from_user(&value,(int32_t*)arg,sizeof(value));
			printk(KERN_INFO "CHL Driver's ioctl(): Value written = %d\n ",value);
			break;
		case RD_VALUE	:
			copy_to_user((int32_t*)arg,&value,sizeof(value));
			printk(KERN_INFO "CHL Driver's ioctl(): Value read = %d\n ",value);
			break;
		default	:
			printk(KERN_INFO "CHL Driver's ioctl(): Invalid IOCTL command\n");
			return -1;
			break;
	}
	return 0;
}



/*****************************************************************/

/*****************Procfs function definitions*********************/
static struct file_operations proc_fops ={
	.open	=	open_proc,
	.read		=	read_proc,
	.write	=	write_proc,
	.release	=	release_proc
};

static int open_proc(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "CHL driver's open_proc(): proc file opened\n");
	return 0;
}

static int release_proc(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "CHL driver's release_proc(): proc file closed\n");
	return 0;
}

static ssize_t read_proc(struct file *filep,char __user *buffer,size_t length,loff_t *offset)
{
	printk(KERN_INFO "CHL driver's read_proc(): proc file reading\n");
	if(len){
		len=0;
	}
	else{
		len=1;
		return 0;
	}

	copy_to_user(buffer,chl_array,20);

	return length;
}

static ssize_t write_proc(struct file *filep,const char __user *buffer,size_t length,loff_t *offset)
{
	printk(KERN_INFO "CHL driver's write_proc(): proc file writing\n");
	if(length <= 20 && length > 0){
		copy_from_user(chl_array,buffer,length);
		return length;
	}
	else{
		printk(KERN_INFO "CHL driver's write_proc(): Invalid size of buffer to write\n");
		return -1;
	}
}

/*****************************************************************/

/* init function of driver */
static int __init chl_init(void)
{

	/* Hard coded values,
		 250 may vary dynamically from system to system */
	int major	=	250;
	int minor	=	0;

	device_number = MKDEV(major,minor);

	/* Allocating major and minor number */
	if(alloc_chrdev_region(&device_number,0,1,"CHL") < 0){
		printk(KERN_INFO "CHL Driver's init() : Unable to allocate major and minor number\n");
		return -1;
	}

	/*  Creating device class */
	if((my_device_class_ptr=class_create(THIS_MODULE,"chl_chardev_class")) 
			== NULL){
		/* ERROR IN CREATING DEVICE CLASS OCCURED */
		printk(KERN_INFO "CHL Driver's init() : Unable to create device class\n");
		goto DEVICE_CLASS_CREATION_ERROR;
	}

	/* Creating device file */
	if(device_create(my_device_class_ptr,NULL,device_number,NULL,"chl_chardev")
			== NULL){
		/* ERROR IN CREATING DEVICE FILE */
		printk(KERN_INFO "CHL Driver's init() : Unable to create device file\n");
		goto DEVICE_CREATE_ERROR;
	}

	/* Initialize file operation for the character device */
	cdev_init(&my_char_dev,&chl_fops);

	/* Add character device to system */
	if(cdev_add(&my_char_dev,device_number,1) == -1)
	{
		/* ERROR IN ADDING CHARACTER DEVICE FILE TO SYSTEM */
		printk(KERN_INFO "CHL Driver's init() : Unable to add character device file to system");
		goto DEVICE_ADD_ERROR;
	}

	/* Creating proc entry */
	proc_create("chl_proc",0666,NULL,&proc_fops);

	printk(KERN_INFO "CHL Driver's init() : Successfully loaded\n");
	return 0;


DEVICE_ADD_ERROR:
	/* Deleting created character device file*/
	device_destroy(my_device_class_ptr,device_number);
DEVICE_CREATE_ERROR:
	/* Destroying the created class */
	class_destroy(my_device_class_ptr);
DEVICE_CLASS_CREATION_ERROR:
	/* Unregistering major and minor number */
	unregister_chrdev_region(device_number,1);
	return -1;

}

/* Exit function of driver */
static void __exit chl_exit(void)
{
	/* Release resources in reverse order of allocation */
	
	/* Remove proc entry */
	remove_proc_entry("chl_proc",NULL);

	/* Removing device from system */
	cdev_del(&my_char_dev);
	
	/* Deleting created character device file */
	device_destroy(my_device_class_ptr,device_number);
	
	/* Destroying the created class */
	class_destroy(my_device_class_ptr);
	
	/* Unregistering major and minor number */
	unregister_chrdev_region(device_number,1);

	printk(KERN_INFO "CHL Driver's exit() : Successfully unloaded");
}

module_init(chl_init);
module_exit(chl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lokesh Chebrolu <lokeshch007@gmail.com>");
MODULE_DESCRIPTION("CHL : Character device driver for file operation");
MODULE_VERSION("0.1");
