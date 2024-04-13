static int inet6_fill_ifacaddr(struct sk_buff *skb, struct ifacaddr6 *ifaca,
				u32 portid, u32 seq, int event, unsigned int flags)
{
	struct nlmsghdr  *nlh;
	u8 scope = RT_SCOPE_UNIVERSE;
	int ifindex = ifaca->aca_idev->dev->ifindex;

	if (ipv6_addr_scope(&ifaca->aca_addr) & IFA_SITE)
		scope = RT_SCOPE_SITE;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct ifaddrmsg), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	put_ifaddrmsg(nlh, 128, IFA_F_PERMANENT, scope, ifindex);
	if (nla_put(skb, IFA_ANYCAST, 16, &ifaca->aca_addr) < 0 ||
	    put_cacheinfo(skb, ifaca->aca_cstamp, ifaca->aca_tstamp,
			  INFINITY_LIFE_TIME, INFINITY_LIFE_TIME) < 0) {
		nlmsg_cancel(skb, nlh);
		return -EMSGSIZE;
	}

	return nlmsg_end(skb, nlh);
}