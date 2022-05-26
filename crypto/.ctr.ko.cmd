cmd_crypto/ctr.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o crypto/ctr.ko crypto/ctr.o crypto/ctr.mod.o ;  true
