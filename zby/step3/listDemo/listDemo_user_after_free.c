#include <linux/module.h>
#include <linux/list.h>
#include "klog.h"
#include <linux/slab.h>

#define NAME_LEN 128

struct my_struct {
	int id;
	char * name;
	struct list_head list;
};

static struct list_head *head = NULL;
static struct list_head *head2 = NULL;
static int count = 0;
static int length = 10;

void create_list(struct list_head *hd)
{
	int i;
	struct my_struct *ptr = NULL;
	for (i = 0; i < length; i++) {
		ptr = kmalloc(sizeof(struct my_struct), GFP_KERNEL);
		if (NULL == ptr) {
			printk(KERN_NOTICE "kmalloc fail in create_list i=%d\n",i);
			return;
		}
		ptr->name = kmalloc(NAME_LEN, GFP_KERNEL);
		if(NULL == ptr->name){
			kfree(ptr);
		}
		ptr->id= ++count;
		sprintf(ptr->name, "list_%d", ptr->id);
		list_add_tail(&ptr->list, hd);
		ptr = NULL;
	}
}

void print_list(struct list_head *hd)
{
	struct my_struct *ptr = NULL;
	struct list_head *p = NULL;
	list_for_each_prev(p, hd) {
		ptr = list_entry(p, struct my_struct, list);
		printk(KERN_NOTICE "%d, %s\n",ptr->id, ptr->name);
	}
}

void free_item(struct my_struct * ptr)
{
	kfree(ptr->name);
	ptr->name = NULL;
	kfree(ptr);
}

void free_list(struct list_head *hd)
{
	struct my_struct *ptr, *n= NULL;
	list_for_each_entry_safe(ptr, n, hd, list) {
		list_del(&ptr->list);
		free_item(ptr);
	}
}

static int __init list_init(void)
{
	/* create a list head */
	LIST_HEAD(_justfortest_);
	head2 = &_justfortest_; 
	create_list(head2);

	head = kmalloc(sizeof(struct list_head), GFP_KERNEL);
	if (NULL == head) {
		printk(KERN_NOTICE "kmalloc fail\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(head);
	create_list(head);
//	list_splice_init(head2, head);
	list_splice(head2, head);
	print_list(head);
	free_list(head);

	print_list(head2);

	return 0;
}

static void __exit list_exit(void)
{
	klog(WARN,"list_exit end\n");
}

MODULE_LICENSE("Dual BSD/GPL");
module_init(list_init);
module_exit(list_exit);
