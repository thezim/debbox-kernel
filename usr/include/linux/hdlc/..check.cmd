cmd_usr/include/linux/hdlc/.check := for f in ioctl.h ; do echo "./usr/include/linux/hdlc/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/linux/hdlc/.check
