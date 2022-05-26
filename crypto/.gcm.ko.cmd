cmd_crypto/gcm.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o crypto/gcm.ko crypto/gcm.o crypto/gcm.mod.o ;  true
