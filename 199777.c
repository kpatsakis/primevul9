static int inet6_fill_ifmcaddr(struct sk_buff *skb, struct ifmcaddr6 *ifmca,
				u32 portid, u32 seq, int event, u16 flags)
{
	struct nlmsghdr  *nlh;
	u8 scope = RT_SCOPE_UNIVERSE;
	int ifindex = ifmca->idev->dev->ifindex;

	if (ipv6_addr_scope(&ifmca->mca_addr) & IFA_SITE)
		scope = RT_SCOPE_SITE;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct ifaddrmsg), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	put_ifaddrmsg(nlh, 128, IFA_F_PERMANENT, scope, ifindex);
	if (nla_put(skb, IFA_MULTICAST, 16, &ifmca->mca_addr) < 0 ||
	    put_cacheinfo(skb, ifmca->mca_cstamp, ifmca->mca_tstamp,
			  INFINITY_LIFE_TIME, INFINITY_LIFE_TIME) < 0) {
		nlmsg_cancel(skb, nlh);
		return -EMSGSIZE;
	}

	return nlmsg_end(skb, nlh);
}