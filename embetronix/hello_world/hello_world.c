#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

static int __init hello_world_init(void)
{
	printk(KERN_INFO "Welcome to hello world driver\n");
	printk(KERN_INFO "My First Linux device driver in 2020\n");
	printk(KERN_INFO "Hello world device driver loaded successfully\n");
	return 0;
}


void __exit hello_world_exit(void)
{
	printk(KERN_INFO "Hello world driver unloaded successfully\n");
	return;
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lokesh <lokeshch007@gmail.com>");
MODULE_DESCRIPTION("Hello world driver");
MODULE_VERSION("0.1");
