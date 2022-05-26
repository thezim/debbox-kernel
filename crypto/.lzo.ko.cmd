cmd_crypto/lzo.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o crypto/lzo.ko crypto/lzo.o crypto/lzo.mod.o ;  true
