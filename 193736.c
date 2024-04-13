static int inet6_rtm_getroute(struct sk_buff *in_skb, struct nlmsghdr* nlh)
{
	struct net *net = sock_net(in_skb->sk);
	struct nlattr *tb[RTA_MAX+1];
	struct rt6_info *rt;
	struct sk_buff *skb;
	struct rtmsg *rtm;
	struct flowi6 fl6;
	int err, iif = 0, oif = 0;

	err = nlmsg_parse(nlh, sizeof(*rtm), tb, RTA_MAX, rtm_ipv6_policy);
	if (err < 0)
		goto errout;

	err = -EINVAL;
	memset(&fl6, 0, sizeof(fl6));

	if (tb[RTA_SRC]) {
		if (nla_len(tb[RTA_SRC]) < sizeof(struct in6_addr))
			goto errout;

		fl6.saddr = *(struct in6_addr *)nla_data(tb[RTA_SRC]);
	}

	if (tb[RTA_DST]) {
		if (nla_len(tb[RTA_DST]) < sizeof(struct in6_addr))
			goto errout;

		fl6.daddr = *(struct in6_addr *)nla_data(tb[RTA_DST]);
	}

	if (tb[RTA_IIF])
		iif = nla_get_u32(tb[RTA_IIF]);

	if (tb[RTA_OIF])
		oif = nla_get_u32(tb[RTA_OIF]);

	if (iif) {
		struct net_device *dev;
		int flags = 0;

		dev = __dev_get_by_index(net, iif);
		if (!dev) {
			err = -ENODEV;
			goto errout;
		}

		fl6.flowi6_iif = iif;

		if (!ipv6_addr_any(&fl6.saddr))
			flags |= RT6_LOOKUP_F_HAS_SADDR;

		rt = (struct rt6_info *)ip6_route_input_lookup(net, dev, &fl6,
							       flags);
	} else {
		fl6.flowi6_oif = oif;

		rt = (struct rt6_info *)ip6_route_output(net, NULL, &fl6);
	}

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb) {
		ip6_rt_put(rt);
		err = -ENOBUFS;
		goto errout;
	}

	/* Reserve room for dummy headers, this skb can pass
	   through good chunk of routing engine.
	 */
	skb_reset_mac_header(skb);
	skb_reserve(skb, MAX_HEADER + sizeof(struct ipv6hdr));

	skb_dst_set(skb, &rt->dst);

	err = rt6_fill_node(net, skb, rt, &fl6.daddr, &fl6.saddr, iif,
			    RTM_NEWROUTE, NETLINK_CB(in_skb).portid,
			    nlh->nlmsg_seq, 0, 0, 0);
	if (err < 0) {
		kfree_skb(skb);
		goto errout;
	}

	err = rtnl_unicast(skb, net, NETLINK_CB(in_skb).portid);
errout:
	return err;
}