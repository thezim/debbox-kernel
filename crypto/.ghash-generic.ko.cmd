cmd_crypto/ghash-generic.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o crypto/ghash-generic.ko crypto/ghash-generic.o crypto/ghash-generic.mod.o ;  true
