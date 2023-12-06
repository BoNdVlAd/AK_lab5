#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct time_log {
	struct list_head entry;
	ktime_t start;
	ktime_t end;
};

MODULE_AUTHOR("Bondarenko Vlad");
MODULE_DESCRIPTION("Lab5");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("1.0");

static LIST_HEAD(time_logs);

static uint counter_value = 1;
module_param(counter_value, uint, 0444);
MODULE_PARM_DESC(counter_value, "Counter Value (p)");

static int __init custom_init(void)
{
	uint i = 0;
	struct time_log *log_entry;

	pr_info("Count: %d\n", counter_value);

	if (!counter_value) {
		pr_warn("The parameter is 0\n");
		return 0;
	}

	if (counter_value >= 5 && counter_value <= 10) {
		pr_warn("The parameter is %d, between 5 and 10, enter less than 5\n", counter_value);
		return 0;
	}

	BUG_ON(counter_value > 10);

	for (i = 0; i < counter_value; i++) {
		log_entry = kmalloc(sizeof(struct time_log), GFP_KERNEL);

		if (i == 5)
			log_entry = NULL;

		if (ZERO_OR_NULL_PTR(log_entry))
			goto exception;

		log_entry->start = ktime_get();
		pr_info("Hello, World\n");
		log_entry->end = ktime_get();

		list_add_tail(&(log_entry->entry), &time_logs);
	}

	return 0;

exception:
	pr_err("The end of memory...\n");
	list_for_each_entry_safe(log_entry, log_entry, &time_logs, entry) {
		list_del(&log_entry->entry);
		kfree(log_entry);
	}

	BUG();
	return -ENOMEM;
}

static void __exit custom_exit(void)
{
	struct time_log *log_entry, *tmp;

	list_for_each_entry_safe(log_entry, tmp, &time_logs, entry) {
		pr_info("Time: %lld",
		        log_entry->end - log_entry->start);

		list_del(&log_entry->entry);
		kfree(log_entry);
	}

	BUG_ON(!list_empty(&time_logs));
}

module_init(custom_init);
module_exit(custom_exit);
