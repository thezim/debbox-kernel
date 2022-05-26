cmd_lib/xxhash.ko := aarch64-linux-gnu-ld -r  -EL  -maarch64linux -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds  --build-id  -o lib/xxhash.ko lib/xxhash.o lib/xxhash.mod.o ;  true
