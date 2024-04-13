static int inet6_rtm_getaddr(struct sk_buff *in_skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(in_skb->sk);
	struct ifaddrmsg *ifm;
	struct nlattr *tb[IFA_MAX+1];
	struct in6_addr *addr = NULL, *peer;
	struct net_device *dev = NULL;
	struct inet6_ifaddr *ifa;
	struct sk_buff *skb;
	int err;

	err = nlmsg_parse(nlh, sizeof(*ifm), tb, IFA_MAX, ifa_ipv6_policy);
	if (err < 0)
		goto errout;

	addr = extract_addr(tb[IFA_ADDRESS], tb[IFA_LOCAL], &peer);
	if (addr == NULL) {
		err = -EINVAL;
		goto errout;
	}

	ifm = nlmsg_data(nlh);
	if (ifm->ifa_index)
		dev = __dev_get_by_index(net, ifm->ifa_index);

	ifa = ipv6_get_ifaddr(net, addr, dev, 1);
	if (!ifa) {
		err = -EADDRNOTAVAIL;
		goto errout;
	}

	skb = nlmsg_new(inet6_ifaddr_msgsize(), GFP_KERNEL);
	if (!skb) {
		err = -ENOBUFS;
		goto errout_ifa;
	}

	err = inet6_fill_ifaddr(skb, ifa, NETLINK_CB(in_skb).portid,
				nlh->nlmsg_seq, RTM_NEWADDR, 0);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in inet6_ifaddr_msgsize() */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout_ifa;
	}
	err = rtnl_unicast(skb, net, NETLINK_CB(in_skb).portid);
errout_ifa:
	in6_ifa_put(ifa);
errout:
	return err;
}