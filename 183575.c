int igmp_rcv(struct sk_buff *skb)
{
	/* This basically follows the spec line by line -- see RFC1112 */
	struct igmphdr *ih;
	struct net_device *dev = skb->dev;
	struct in_device *in_dev;
	int len = skb->len;
	bool dropped = true;

	if (netif_is_l3_master(dev)) {
		dev = dev_get_by_index_rcu(dev_net(dev), IPCB(skb)->iif);
		if (!dev)
			goto drop;
	}

	in_dev = __in_dev_get_rcu(dev);
	if (!in_dev)
		goto drop;

	if (!pskb_may_pull(skb, sizeof(struct igmphdr)))
		goto drop;

	if (skb_checksum_simple_validate(skb))
		goto drop;

	ih = igmp_hdr(skb);
	switch (ih->type) {
	case IGMP_HOST_MEMBERSHIP_QUERY:
		dropped = igmp_heard_query(in_dev, skb, len);
		break;
	case IGMP_HOST_MEMBERSHIP_REPORT:
	case IGMPV2_HOST_MEMBERSHIP_REPORT:
		/* Is it our report looped back? */
		if (rt_is_output_route(skb_rtable(skb)))
			break;
		/* don't rely on MC router hearing unicast reports */
		if (skb->pkt_type == PACKET_MULTICAST ||
		    skb->pkt_type == PACKET_BROADCAST)
			dropped = igmp_heard_report(in_dev, ih->group);
		break;
	case IGMP_PIM:
#ifdef CONFIG_IP_PIMSM_V1
		return pim_rcv_v1(skb);
#endif
	case IGMPV3_HOST_MEMBERSHIP_REPORT:
	case IGMP_DVMRP:
	case IGMP_TRACE:
	case IGMP_HOST_LEAVE_MESSAGE:
	case IGMP_MTRACE:
	case IGMP_MTRACE_RESP:
		break;
	default:
		break;
	}

drop:
	if (dropped)
		kfree_skb(skb);
	else
		consume_skb(skb);
	return 0;
}