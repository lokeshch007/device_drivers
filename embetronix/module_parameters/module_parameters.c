#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>

int var,arr_var[4];
char *string_input;
int var_cb=0;

/** Normal variables **/

module_param(var,int,S_IRUSR|S_IWUSR);
module_param(string_input,charp,S_IRUSR|S_IWUSR);
module_param_array(arr_var,int,NULL,S_IRUSR|S_IWUSR);

/**********************/

/** Callback variable **/

int notify_param(const char *val,const struct kernel_param *kp)
{
	//Need to learn about this line of code
	int res = param_set_int(val,kp);

	if(res==0)
	{
		printk(KERN_INFO "Call back function calledn");
		printk(KERN_INFO "New value of var_cb = %d\n",var_cb);
		return 0;
	}
	return -1;
}

const struct kernel_param_ops my_param_ops =
{
	.set = &notify_param, // User setter assigning
	.get = &param_get_int, //Assign standard getter
};

module_param_cb(var_cb,&my_param_ops,&var_cb,S_IRUSR|S_IWUSR);

/***********************/


static int __init hello_world_init(void)
{
	int i;
	printk(KERN_INFO "Var = %d\n",var);
	printk(KERN_INFO "var_cb = %d\n",var_cb);
	printk(KERN_INFO "string_input = %s\n",string_input);
	for(i=0;i<(sizeof(arr_var)/sizeof(int));i++)
	{
		printk(KERN_INFO "arr_var[%d] = %d\n",i,arr_var[i]);
	}
	printk(KERN_INFO "Module Parameters module inserted successfully\n");
	return 0;
}

void  __exit hello_world_exit(void)
{
	printk(KERN_INFO "Module parameters module removed successfully\n");
	return;
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lokesh <lokeshch007@gmail.com>");
MODULE_DESCRIPTION("Module parameters demo program");
MODULE_VERSION("0.1");
