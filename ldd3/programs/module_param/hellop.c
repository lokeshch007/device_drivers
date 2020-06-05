#include<linux/init.h>
#include<linux/module.h>
#include<linux/sched.h>
#include<linux/moduleparam.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
static int array_num[100]={0,};

module_param(howmany, int,S_IRUGO);
module_param(whom, charp,S_IRUGO);
module_param_array(array_num,int,NULL,S_IRUGO);

static int hello_init(void)
{
	int i=0;
	for(i=0;i<howmany;i++){
		printk(KERN_ALERT "Hello, %s, %d\n",whom,array_num[i]);
	}
	printk(KERN_INFO "The process is \"%s\" (pid %i)\n",current->comm,current->pid);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);
