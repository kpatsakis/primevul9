static int igmp_send_report(struct in_device *in_dev, struct ip_mc_list *pmc,
	int type)
{
	struct sk_buff *skb;
	struct iphdr *iph;
	struct igmphdr *ih;
	struct rtable *rt;
	struct net_device *dev = in_dev->dev;
	struct net *net = dev_net(dev);
	__be32	group = pmc ? pmc->multiaddr : 0;
	struct flowi4 fl4;
	__be32	dst;
	int hlen, tlen;

	if (type == IGMPV3_HOST_MEMBERSHIP_REPORT)
		return igmpv3_send_report(in_dev, pmc);

	if (ipv4_is_local_multicast(group) && !net->ipv4.sysctl_igmp_llm_reports)
		return 0;

	if (type == IGMP_HOST_LEAVE_MESSAGE)
		dst = IGMP_ALL_ROUTER;
	else
		dst = group;

	rt = ip_route_output_ports(net, &fl4, NULL, dst, 0,
				   0, 0,
				   IPPROTO_IGMP, 0, dev->ifindex);
	if (IS_ERR(rt))
		return -1;

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	skb = alloc_skb(IGMP_SIZE + hlen + tlen, GFP_ATOMIC);
	if (!skb) {
		ip_rt_put(rt);
		return -1;
	}
	skb->priority = TC_PRIO_CONTROL;

	skb_dst_set(skb, &rt->dst);

	skb_reserve(skb, hlen);

	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	skb_put(skb, sizeof(struct iphdr) + 4);

	iph->version  = 4;
	iph->ihl      = (sizeof(struct iphdr)+4)>>2;
	iph->tos      = 0xc0;
	iph->frag_off = htons(IP_DF);
	iph->ttl      = 1;
	iph->daddr    = dst;
	iph->saddr    = fl4.saddr;
	iph->protocol = IPPROTO_IGMP;
	ip_select_ident(net, skb, NULL);
	((u8 *)&iph[1])[0] = IPOPT_RA;
	((u8 *)&iph[1])[1] = 4;
	((u8 *)&iph[1])[2] = 0;
	((u8 *)&iph[1])[3] = 0;

	ih = skb_put(skb, sizeof(struct igmphdr));
	ih->type = type;
	ih->code = 0;
	ih->csum = 0;
	ih->group = group;
	ih->csum = ip_compute_csum((void *)ih, sizeof(struct igmphdr));

	return ip_local_out(net, skb->sk, skb);
}