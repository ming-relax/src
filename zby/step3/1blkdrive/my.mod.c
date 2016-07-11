#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xf8e3dbd2, "struct_module" },
	{ 0x510c8eb3, "alloc_disk" },
	{ 0xcca60e72, "bio_alloc" },
	{ 0x89892632, "malloc_sizes" },
	{ 0x429328d9, "_spin_lock" },
	{ 0xdd5a37a7, "_spin_lock_irqsave" },
	{ 0xde0bdcff, "memset" },
	{ 0x8267a42e, "blk_alloc_queue" },
	{ 0x5aa5e8ff, "blk_put_queue" },
	{ 0xdd132261, "printk" },
	{ 0x3be98e3f, "kthread_stop" },
	{ 0x79aa9e48, "del_gendisk" },
	{ 0x74fb4a6a, "open_bdev_excl" },
	{ 0x71a50dbc, "register_blkdev" },
	{ 0xdf15005, "_spin_unlock_irqrestore" },
	{ 0x2f8a3930, "bio_endio" },
	{ 0xd742ec4, "bio_put" },
	{ 0xeac1c4af, "unregister_blkdev" },
	{ 0x9053842b, "blk_queue_hardsect_size" },
	{ 0x60c50a62, "submit_bio" },
	{ 0x4c503ced, "kmem_cache_alloc" },
	{ 0x5a9822da, "blk_queue_make_request" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0xf8d01868, "put_disk" },
	{ 0xe2245b73, "wake_up_process" },
	{ 0xd2965f6f, "kthread_should_stop" },
	{ 0x37a0cba, "kfree" },
	{ 0x6d7b5195, "kthread_create" },
	{ 0x34548ad1, "add_disk" },
	{ 0xaf25400d, "snprintf" },
	{ 0x8f84d99, "close_bdev_excl" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "503563110B54A64BFDA6A4E");
