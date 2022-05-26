cmd_usr/include/misc/.check := for f in cxl.h ocxl.h ; do echo "./usr/include/misc/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/misc/.check
