static void rt6_do_redirect(struct dst_entry *dst, struct sock *sk, struct sk_buff *skb)
{
	struct net *net = dev_net(skb->dev);
	struct netevent_redirect netevent;
	struct rt6_info *rt, *nrt = NULL;
	struct ndisc_options ndopts;
	struct inet6_dev *in6_dev;
	struct neighbour *neigh;
	struct rd_msg *msg;
	int optlen, on_link;
	u8 *lladdr;

	optlen = skb_tail_pointer(skb) - skb_transport_header(skb);
	optlen -= sizeof(*msg);

	if (optlen < 0) {
		net_dbg_ratelimited("rt6_do_redirect: packet too short\n");
		return;
	}

	msg = (struct rd_msg *)icmp6_hdr(skb);

	if (ipv6_addr_is_multicast(&msg->dest)) {
		net_dbg_ratelimited("rt6_do_redirect: destination address is multicast\n");
		return;
	}

	on_link = 0;
	if (ipv6_addr_equal(&msg->dest, &msg->target)) {
		on_link = 1;
	} else if (ipv6_addr_type(&msg->target) !=
		   (IPV6_ADDR_UNICAST|IPV6_ADDR_LINKLOCAL)) {
		net_dbg_ratelimited("rt6_do_redirect: target address is not link-local unicast\n");
		return;
	}

	in6_dev = __in6_dev_get(skb->dev);
	if (!in6_dev)
		return;
	if (in6_dev->cnf.forwarding || !in6_dev->cnf.accept_redirects)
		return;

	/* RFC2461 8.1:
	 *	The IP source address of the Redirect MUST be the same as the current
	 *	first-hop router for the specified ICMP Destination Address.
	 */

	if (!ndisc_parse_options(msg->opt, optlen, &ndopts)) {
		net_dbg_ratelimited("rt6_redirect: invalid ND options\n");
		return;
	}

	lladdr = NULL;
	if (ndopts.nd_opts_tgt_lladdr) {
		lladdr = ndisc_opt_addr_data(ndopts.nd_opts_tgt_lladdr,
					     skb->dev);
		if (!lladdr) {
			net_dbg_ratelimited("rt6_redirect: invalid link-layer address length\n");
			return;
		}
	}

	rt = (struct rt6_info *) dst;
	if (rt == net->ipv6.ip6_null_entry) {
		net_dbg_ratelimited("rt6_redirect: source isn't a valid nexthop for redirect target\n");
		return;
	}

	/* Redirect received -> path was valid.
	 * Look, redirects are sent only in response to data packets,
	 * so that this nexthop apparently is reachable. --ANK
	 */
	dst_confirm(&rt->dst);

	neigh = __neigh_lookup(&nd_tbl, &msg->target, skb->dev, 1);
	if (!neigh)
		return;

	/*
	 *	We have finally decided to accept it.
	 */

	neigh_update(neigh, lladdr, NUD_STALE,
		     NEIGH_UPDATE_F_WEAK_OVERRIDE|
		     NEIGH_UPDATE_F_OVERRIDE|
		     (on_link ? 0 : (NEIGH_UPDATE_F_OVERRIDE_ISROUTER|
				     NEIGH_UPDATE_F_ISROUTER))
		     );

	nrt = ip6_rt_copy(rt, &msg->dest);
	if (!nrt)
		goto out;

	nrt->rt6i_flags = RTF_GATEWAY|RTF_UP|RTF_DYNAMIC|RTF_CACHE;
	if (on_link)
		nrt->rt6i_flags &= ~RTF_GATEWAY;

	nrt->rt6i_gateway = *(struct in6_addr *)neigh->primary_key;

	if (ip6_ins_rt(nrt))
		goto out;

	netevent.old = &rt->dst;
	netevent.new = &nrt->dst;
	netevent.daddr = &msg->dest;
	netevent.neigh = neigh;
	call_netevent_notifiers(NETEVENT_REDIRECT, &netevent);

	if (rt->rt6i_flags & RTF_CACHE) {
		rt = (struct rt6_info *) dst_clone(&rt->dst);
		ip6_del_rt(rt);
	}

out:
	neigh_release(neigh);
}