cmd_kernel/dma/built-in.a := rm -f kernel/dma/built-in.a; aarch64-linux-gnu-ar rcSTPD kernel/dma/built-in.a kernel/dma/mapping.o kernel/dma/coherent.o kernel/dma/direct.o kernel/dma/swiotlb.o
