cmd_usr/include/video/.check := for f in uvesafb.h edid.h sisfb.h ; do echo "./usr/include/video/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/video/.check
