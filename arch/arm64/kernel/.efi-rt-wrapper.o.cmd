cmd_arch/arm64/kernel/efi-rt-wrapper.o := aarch64-linux-gnu-gcc -Wp,-MD,arch/arm64/kernel/.efi-rt-wrapper.o.d  -nostdinc -isystem /usr/lib/gcc-cross/aarch64-linux-gnu/6/include -I./arch/arm64/include -I./arch/arm64/include/generated  -I./include -I./arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -D__ASSEMBLY__ -fno-PIE -DCONFIG_AS_LSE=1 -mabi=lp64   -c -o arch/arm64/kernel/efi-rt-wrapper.o arch/arm64/kernel/efi-rt-wrapper.S

source_arch/arm64/kernel/efi-rt-wrapper.o := arch/arm64/kernel/efi-rt-wrapper.S

deps_arch/arm64/kernel/efi-rt-wrapper.o := \
  include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  include/linux/linkage.h \
  include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/rel/crcs.h) \
    $(wildcard include/config/have/arch/prel32/relocations.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
    $(wildcard include/config/unused/symbols.h) \
  arch/arm64/include/asm/linkage.h \

arch/arm64/kernel/efi-rt-wrapper.o: $(deps_arch/arm64/kernel/efi-rt-wrapper.o)

$(deps_arch/arm64/kernel/efi-rt-wrapper.o):
