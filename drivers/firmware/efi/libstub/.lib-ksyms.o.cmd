cmd_drivers/firmware/efi/libstub/lib-ksyms.o := aarch64-linux-gnu-objdump -h drivers/firmware/efi/libstub/lib.a | sed -ne '/___ksymtab/s/.*+\([^ ]*\).*/EXTERN(\1)/p' >drivers/firmware/efi/libstub/.lib-ksyms.o.lds; rm -f drivers/firmware/efi/libstub/.lib_exports.o; echo | aarch64-linux-gnu-gcc -Wp,-MD,drivers/firmware/efi/libstub/.lib-ksyms.o.d  -nostdinc -isystem /usr/lib/gcc-cross/aarch64-linux-gnu/6/include -I./arch/arm64/include -I./arch/arm64/include/generated  -I./include -I./arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -D__ASSEMBLY__ -fno-PIE -DCONFIG_AS_LSE=1 -mabi=lp64   -c -o drivers/firmware/efi/libstub/.lib_exports.o -x assembler -; aarch64-linux-gnu-ld  -EL  -maarch64linux    -r -o drivers/firmware/efi/libstub/lib-ksyms.o -T drivers/firmware/efi/libstub/.lib-ksyms.o.lds drivers/firmware/efi/libstub/.lib_exports.o; rm drivers/firmware/efi/libstub/.lib_exports.o drivers/firmware/efi/libstub/.lib-ksyms.o.lds