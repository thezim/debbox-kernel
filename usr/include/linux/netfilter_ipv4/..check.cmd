cmd_usr/include/linux/netfilter_ipv4/.check := for f in ipt_CLUSTERIP.h ipt_TTL.h ipt_ecn.h ip_tables.h ipt_LOG.h ipt_REJECT.h ipt_ECN.h ipt_ah.h ipt_ttl.h ; do echo "./usr/include/linux/netfilter_ipv4/$${f}"; done | xargs perl scripts/headers_check.pl ./usr/include arm64; touch usr/include/linux/netfilter_ipv4/.check
