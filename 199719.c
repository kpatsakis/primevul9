static inline size_t inet6_ifla6_size(void)
{
	return nla_total_size(4) /* IFLA_INET6_FLAGS */
	     + nla_total_size(sizeof(struct ifla_cacheinfo))
	     + nla_total_size(DEVCONF_MAX * 4) /* IFLA_INET6_CONF */
	     + nla_total_size(IPSTATS_MIB_MAX * 8) /* IFLA_INET6_STATS */
	     + nla_total_size(ICMP6_MIB_MAX * 8) /* IFLA_INET6_ICMP6STATS */
	     + nla_total_size(sizeof(struct in6_addr)); /* IFLA_INET6_TOKEN */
}