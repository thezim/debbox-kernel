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

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00001095d00003124sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00003124sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001095d00003132sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001095d00000242sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001095d00000244sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001095d00003131sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001095d00003531sv*sd*bc*sc*i*");
