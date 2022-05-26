cmd_usr/include/linux/isdn/.check := for f in capicmd.h ; do echo "./usr/include/linux/isdn/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/linux/isdn/.check
