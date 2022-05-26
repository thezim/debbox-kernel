cmd_drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o := aarch64-linux-gnu-gcc -Wp,-MD,drivers/soc/alpine/HAL/services/sys_fabric/.al_init_neon_aarch32.o.d  -nostdinc -isystem /usr/lib/gcc-cross/aarch64-linux-gnu/6/include -I./arch/arm64/include -I./arch/arm64/include/generated  -I./include -I./arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -D__ASSEMBLY__ -fno-PIE -DCONFIG_AS_LSE=1 -mabi=lp64   -c -o drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.S

source_drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o := drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.S

deps_drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o := \
  include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \

drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o: $(deps_drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o)

$(deps_drivers/soc/alpine/HAL/services/sys_fabric/al_init_neon_aarch32.o):
