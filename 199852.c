static inline int ipv6_saddr_preferred(int type)
{
	if (type & (IPV6_ADDR_MAPPED|IPV6_ADDR_COMPATv4|IPV6_ADDR_LOOPBACK))
		return 1;
	return 0;
}