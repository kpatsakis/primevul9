static int inet6_netconf_fill_devconf(struct sk_buff *skb, int ifindex,
				      struct ipv6_devconf *devconf, u32 portid,
				      u32 seq, int event, unsigned int flags,
				      int type)
{
	struct nlmsghdr  *nlh;
	struct netconfmsg *ncm;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct netconfmsg),
			flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	ncm = nlmsg_data(nlh);
	ncm->ncm_family = AF_INET6;

	if (nla_put_s32(skb, NETCONFA_IFINDEX, ifindex) < 0)
		goto nla_put_failure;

	/* type -1 is used for ALL */
	if ((type == -1 || type == NETCONFA_FORWARDING) &&
	    nla_put_s32(skb, NETCONFA_FORWARDING, devconf->forwarding) < 0)
		goto nla_put_failure;
#ifdef CONFIG_IPV6_MROUTE
	if ((type == -1 || type == NETCONFA_MC_FORWARDING) &&
	    nla_put_s32(skb, NETCONFA_MC_FORWARDING,
			devconf->mc_forwarding) < 0)
		goto nla_put_failure;
#endif
	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}