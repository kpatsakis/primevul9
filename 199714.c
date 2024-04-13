inet6_rtm_newaddr(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	struct ifaddrmsg *ifm;
	struct nlattr *tb[IFA_MAX+1];
	struct in6_addr *pfx, *peer_pfx;
	struct inet6_ifaddr *ifa;
	struct net_device *dev;
	u32 valid_lft = INFINITY_LIFE_TIME, preferred_lft = INFINITY_LIFE_TIME;
	u8 ifa_flags;
	int err;

	err = nlmsg_parse(nlh, sizeof(*ifm), tb, IFA_MAX, ifa_ipv6_policy);
	if (err < 0)
		return err;

	ifm = nlmsg_data(nlh);
	pfx = extract_addr(tb[IFA_ADDRESS], tb[IFA_LOCAL], &peer_pfx);
	if (pfx == NULL)
		return -EINVAL;

	if (tb[IFA_CACHEINFO]) {
		struct ifa_cacheinfo *ci;

		ci = nla_data(tb[IFA_CACHEINFO]);
		valid_lft = ci->ifa_valid;
		preferred_lft = ci->ifa_prefered;
	} else {
		preferred_lft = INFINITY_LIFE_TIME;
		valid_lft = INFINITY_LIFE_TIME;
	}

	dev =  __dev_get_by_index(net, ifm->ifa_index);
	if (dev == NULL)
		return -ENODEV;

	/* We ignore other flags so far. */
	ifa_flags = ifm->ifa_flags & (IFA_F_NODAD | IFA_F_HOMEADDRESS);

	ifa = ipv6_get_ifaddr(net, pfx, dev, 1);
	if (ifa == NULL) {
		/*
		 * It would be best to check for !NLM_F_CREATE here but
		 * userspace alreay relies on not having to provide this.
		 */
		return inet6_addr_add(net, ifm->ifa_index, pfx, peer_pfx,
				      ifm->ifa_prefixlen, ifa_flags,
				      preferred_lft, valid_lft);
	}

	if (nlh->nlmsg_flags & NLM_F_EXCL ||
	    !(nlh->nlmsg_flags & NLM_F_REPLACE))
		err = -EEXIST;
	else
		err = inet6_addr_modify(ifa, ifa_flags, preferred_lft, valid_lft);

	in6_ifa_put(ifa);

	return err;
}