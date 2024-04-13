static int inet6_netconf_msgsize_devconf(int type)
{
	int size =  NLMSG_ALIGN(sizeof(struct netconfmsg))
		    + nla_total_size(4);	/* NETCONFA_IFINDEX */

	/* type -1 is used for ALL */
	if (type == -1 || type == NETCONFA_FORWARDING)
		size += nla_total_size(4);
#ifdef CONFIG_IPV6_MROUTE
	if (type == -1 || type == NETCONFA_MC_FORWARDING)
		size += nla_total_size(4);
#endif

	return size;
}