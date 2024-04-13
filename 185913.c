static int rt6_fill_node(struct net *net,
			 struct sk_buff *skb, struct rt6_info *rt,
			 struct in6_addr *dst, struct in6_addr *src,
			 int iif, int type, u32 portid, u32 seq,
			 int prefix, int nowait, unsigned int flags)
{
	struct rtmsg *rtm;
	struct nlmsghdr *nlh;
	long expires;
	u32 table;

	if (prefix) {	/* user wants prefix routes only */
		if (!(rt->rt6i_flags & RTF_PREFIX_RT)) {
			/* success since this is not a prefix route */
			return 1;
		}
	}

	nlh = nlmsg_put(skb, portid, seq, type, sizeof(*rtm), flags);
	if (!nlh)
		return -EMSGSIZE;

	rtm = nlmsg_data(nlh);
	rtm->rtm_family = AF_INET6;
	rtm->rtm_dst_len = rt->rt6i_dst.plen;
	rtm->rtm_src_len = rt->rt6i_src.plen;
	rtm->rtm_tos = 0;
	if (rt->rt6i_table)
		table = rt->rt6i_table->tb6_id;
	else
		table = RT6_TABLE_UNSPEC;
	rtm->rtm_table = table;
	if (nla_put_u32(skb, RTA_TABLE, table))
		goto nla_put_failure;
	if (rt->rt6i_flags & RTF_REJECT) {
		switch (rt->dst.error) {
		case -EINVAL:
			rtm->rtm_type = RTN_BLACKHOLE;
			break;
		case -EACCES:
			rtm->rtm_type = RTN_PROHIBIT;
			break;
		case -EAGAIN:
			rtm->rtm_type = RTN_THROW;
			break;
		default:
			rtm->rtm_type = RTN_UNREACHABLE;
			break;
		}
	}
	else if (rt->rt6i_flags & RTF_LOCAL)
		rtm->rtm_type = RTN_LOCAL;
	else if (rt->dst.dev && (rt->dst.dev->flags & IFF_LOOPBACK))
		rtm->rtm_type = RTN_LOCAL;
	else
		rtm->rtm_type = RTN_UNICAST;
	rtm->rtm_flags = 0;
	rtm->rtm_scope = RT_SCOPE_UNIVERSE;
	rtm->rtm_protocol = rt->rt6i_protocol;
	if (rt->rt6i_flags & RTF_DYNAMIC)
		rtm->rtm_protocol = RTPROT_REDIRECT;
	else if (rt->rt6i_flags & RTF_ADDRCONF) {
		if (rt->rt6i_flags & (RTF_DEFAULT | RTF_ROUTEINFO))
			rtm->rtm_protocol = RTPROT_RA;
		else
			rtm->rtm_protocol = RTPROT_KERNEL;
	}

	if (rt->rt6i_flags & RTF_CACHE)
		rtm->rtm_flags |= RTM_F_CLONED;

	if (dst) {
		if (nla_put(skb, RTA_DST, 16, dst))
			goto nla_put_failure;
		rtm->rtm_dst_len = 128;
	} else if (rtm->rtm_dst_len)
		if (nla_put(skb, RTA_DST, 16, &rt->rt6i_dst.addr))
			goto nla_put_failure;
#ifdef CONFIG_IPV6_SUBTREES
	if (src) {
		if (nla_put(skb, RTA_SRC, 16, src))
			goto nla_put_failure;
		rtm->rtm_src_len = 128;
	} else if (rtm->rtm_src_len &&
		   nla_put(skb, RTA_SRC, 16, &rt->rt6i_src.addr))
		goto nla_put_failure;
#endif
	if (iif) {
#ifdef CONFIG_IPV6_MROUTE
		if (ipv6_addr_is_multicast(&rt->rt6i_dst.addr)) {
			int err = ip6mr_get_route(net, skb, rtm, nowait);
			if (err <= 0) {
				if (!nowait) {
					if (err == 0)
						return 0;
					goto nla_put_failure;
				} else {
					if (err == -EMSGSIZE)
						goto nla_put_failure;
				}
			}
		} else
#endif
			if (nla_put_u32(skb, RTA_IIF, iif))
				goto nla_put_failure;
	} else if (dst) {
		struct in6_addr saddr_buf;
		if (ip6_route_get_saddr(net, rt, dst, 0, &saddr_buf) == 0 &&
		    nla_put(skb, RTA_PREFSRC, 16, &saddr_buf))
			goto nla_put_failure;
	}

	if (rt->rt6i_prefsrc.plen) {
		struct in6_addr saddr_buf;
		saddr_buf = rt->rt6i_prefsrc.addr;
		if (nla_put(skb, RTA_PREFSRC, 16, &saddr_buf))
			goto nla_put_failure;
	}

	if (rtnetlink_put_metrics(skb, dst_metrics_ptr(&rt->dst)) < 0)
		goto nla_put_failure;

	if (rt->rt6i_flags & RTF_GATEWAY) {
		if (nla_put(skb, RTA_GATEWAY, 16, &rt->rt6i_gateway) < 0)
			goto nla_put_failure;
	}

	if (rt->dst.dev &&
	    nla_put_u32(skb, RTA_OIF, rt->dst.dev->ifindex))
		goto nla_put_failure;
	if (nla_put_u32(skb, RTA_PRIORITY, rt->rt6i_metric))
		goto nla_put_failure;

	expires = (rt->rt6i_flags & RTF_EXPIRES) ? rt->dst.expires - jiffies : 0;

	if (rtnl_put_cacheinfo(skb, &rt->dst, 0, expires, rt->dst.error) < 0)
		goto nla_put_failure;

	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}