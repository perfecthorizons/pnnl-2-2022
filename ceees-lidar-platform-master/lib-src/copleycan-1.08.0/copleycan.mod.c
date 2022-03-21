#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x80c8b05d, "module_layout" },
	{ 0xf4e5e0ab, "kobject_put" },
	{ 0x489c6f18, "cdev_del" },
	{ 0xdf539367, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x60cd5082, "cdev_init" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xe3b02ae1, "pci_disable_device" },
	{ 0xd0d9eeb6, "down_interruptible" },
	{ 0xa5afa2f1, "usb_kill_urb" },
	{ 0x38142440, "device_destroy" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xad4f3835, "pci_release_regions" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xd697e69a, "trace_hardirqs_on" },
	{ 0x526c3a6c, "jiffies" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0x5bbe49f4, "__init_waitqueue_head" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x5f754e5a, "memset" },
	{ 0xb3f54f76, "pci_restore_state" },
	{ 0x39a12ca7, "_raw_spin_unlock_irqrestore" },
	{ 0x634acce4, "usb_deregister" },
	{ 0x7c32d0f0, "printk" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x1d37eeed, "ioremap" },
	{ 0x2a58285a, "usb_set_interface" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x9b884788, "usb_control_msg" },
	{ 0x69e1d2e, "device_create" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0xc40ee47d, "usb_free_coherent" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xab9fff11, "cdev_add" },
	{ 0xa9217b9b, "usb_submit_urb" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x86a4889a, "kmalloc_order_trace" },
	{ 0xdd452e5a, "pci_unregister_driver" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x80fc0c5, "kmem_cache_alloc_trace" },
	{ 0x5f849a69, "_raw_spin_lock_irqsave" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x3dcf1ffa, "__wake_up" },
	{ 0x647af474, "prepare_to_wait_event" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x63ef84d5, "pci_request_regions" },
	{ 0xedc03953, "iounmap" },
	{ 0x581cde4e, "up" },
	{ 0x6a18c6b5, "__pci_register_driver" },
	{ 0x56287daf, "usb_register_driver" },
	{ 0x5868fd99, "class_destroy" },
	{ 0x49970de8, "finish_wait" },
	{ 0x216d759a, "mmiocpy" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x8e856de9, "usb_alloc_coherent" },
	{ 0x44b02d02, "pci_enable_device" },
	{ 0xec3d2e1b, "trace_hardirqs_off" },
	{ 0x9c921cae, "__class_create" },
	{ 0x92d97238, "usb_free_urb" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x4214105e, "usb_alloc_urb" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x419ffe8a, "pci_save_state" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "672A61F9156798303DFE4B1");
