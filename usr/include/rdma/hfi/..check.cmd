cmd_usr/include/rdma/hfi/.check := for f in hfi1_ioctl.h hfi1_user.h ; do echo "./usr/include/rdma/hfi/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/rdma/hfi/.check