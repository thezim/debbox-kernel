cmd_drivers/net/bonding/bonding.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o drivers/net/bonding/bonding.ko drivers/net/bonding/bonding.o drivers/net/bonding/bonding.mod.o ;  true
