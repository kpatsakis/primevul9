static int ip_mc_check_igmp_query(struct sk_buff *skb)
{
	unsigned int transport_len = ip_transport_len(skb);
	unsigned int len;

	/* IGMPv{1,2}? */
	if (transport_len != sizeof(struct igmphdr)) {
		/* or IGMPv3? */
		if (transport_len < sizeof(struct igmpv3_query))
			return -EINVAL;

		len = skb_transport_offset(skb) + sizeof(struct igmpv3_query);
		if (!ip_mc_may_pull(skb, len))
			return -EINVAL;
	}

	/* RFC2236+RFC3376 (IGMPv2+IGMPv3) require the multicast link layer
	 * all-systems destination addresses (224.0.0.1) for general queries
	 */
	if (!igmp_hdr(skb)->group &&
	    ip_hdr(skb)->daddr != htonl(INADDR_ALLHOSTS_GROUP))
		return -EINVAL;

	return 0;
}