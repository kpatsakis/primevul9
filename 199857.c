static int inet6_fill_ifla6_attrs(struct sk_buff *skb, struct inet6_dev *idev)
{
	struct nlattr *nla;
	struct ifla_cacheinfo ci;

	if (nla_put_u32(skb, IFLA_INET6_FLAGS, idev->if_flags))
		goto nla_put_failure;
	ci.max_reasm_len = IPV6_MAXPLEN;
	ci.tstamp = cstamp_delta(idev->tstamp);
	ci.reachable_time = jiffies_to_msecs(idev->nd_parms->reachable_time);
	ci.retrans_time = jiffies_to_msecs(idev->nd_parms->retrans_time);
	if (nla_put(skb, IFLA_INET6_CACHEINFO, sizeof(ci), &ci))
		goto nla_put_failure;
	nla = nla_reserve(skb, IFLA_INET6_CONF, DEVCONF_MAX * sizeof(s32));
	if (nla == NULL)
		goto nla_put_failure;
	ipv6_store_devconf(&idev->cnf, nla_data(nla), nla_len(nla));

	/* XXX - MC not implemented */

	nla = nla_reserve(skb, IFLA_INET6_STATS, IPSTATS_MIB_MAX * sizeof(u64));
	if (nla == NULL)
		goto nla_put_failure;
	snmp6_fill_stats(nla_data(nla), idev, IFLA_INET6_STATS, nla_len(nla));

	nla = nla_reserve(skb, IFLA_INET6_ICMP6STATS, ICMP6_MIB_MAX * sizeof(u64));
	if (nla == NULL)
		goto nla_put_failure;
	snmp6_fill_stats(nla_data(nla), idev, IFLA_INET6_ICMP6STATS, nla_len(nla));

	nla = nla_reserve(skb, IFLA_INET6_TOKEN, sizeof(struct in6_addr));
	if (nla == NULL)
		goto nla_put_failure;
	read_lock_bh(&idev->lock);
	memcpy(nla_data(nla), idev->token.s6_addr, nla_len(nla));
	read_unlock_bh(&idev->lock);

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}