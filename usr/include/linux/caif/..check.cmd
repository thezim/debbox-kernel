cmd_usr/include/linux/caif/.check := for f in if_caif.h caif_socket.h ; do echo "./usr/include/linux/caif/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/linux/caif/.check