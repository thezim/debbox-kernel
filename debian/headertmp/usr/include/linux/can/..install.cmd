cmd_debian/headertmp/usr/include/linux/can/.install := /bin/bash scripts/headers_install.sh ./debian/headertmp/usr/include/linux/can ./include/uapi/linux/can bcm.h vxcan.h gw.h raw.h netlink.h error.h; /bin/bash scripts/headers_install.sh ./debian/headertmp/usr/include/linux/can ./include/generated/uapi/linux/can ; touch debian/headertmp/usr/include/linux/can/.install