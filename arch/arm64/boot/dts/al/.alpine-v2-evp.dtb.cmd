cmd_arch/arm64/boot/dts/al/alpine-v2-evp.dtb := mkdir -p arch/arm64/boot/dts/al/ ; aarch64-linux-gnu-gcc -E -Wp,-MD,arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.d.pre.tmp -nostdinc -I./scripts/dtc/include-prefixes -undef -D__DTS__ -x assembler-with-cpp -o arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.dts.tmp arch/arm64/boot/dts/al/alpine-v2-evp.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm64/boot/dts/al/alpine-v2-evp.dtb -b 0 -iarch/arm64/boot/dts/al/ -i./scripts/dtc/include-prefixes -Wno-unit_address_vs_reg -Wno-unit_address_format -Wno-avoid_unnecessary_addr_size -Wno-alias_paths -Wno-graph_child_address -Wno-graph_port -Wno-simple_bus_reg -Wno-unique_unit_address -Wno-pci_device_reg  -d arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.d.dtc.tmp arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.dts.tmp ; cat arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.d.pre.tmp arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.d.dtc.tmp > arch/arm64/boot/dts/al/.alpine-v2-evp.dtb.d

source_arch/arm64/boot/dts/al/alpine-v2-evp.dtb := arch/arm64/boot/dts/al/alpine-v2-evp.dts

deps_arch/arm64/boot/dts/al/alpine-v2-evp.dtb := \
  arch/arm64/boot/dts/al/alpine-v2.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/interrupt-controller/arm-gic.h \
  scripts/dtc/include-prefixes/dt-bindings/interrupt-controller/irq.h \

arch/arm64/boot/dts/al/alpine-v2-evp.dtb: $(deps_arch/arm64/boot/dts/al/alpine-v2-evp.dtb)

$(deps_arch/arm64/boot/dts/al/alpine-v2-evp.dtb):
