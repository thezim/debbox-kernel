cmd_drivers/net/mii.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o drivers/net/mii.ko drivers/net/mii.o drivers/net/mii.mod.o ;  true