cmd_net/ipv6/ah6.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o net/ipv6/ah6.ko net/ipv6/ah6.o net/ipv6/ah6.mod.o ;  true