cmd_usr/include/linux/sunrpc/.check := for f in debug.h ; do echo "./usr/include/linux/sunrpc/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/linux/sunrpc/.check
