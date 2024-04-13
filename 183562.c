static int ip_mc_check_igmp_csum(struct sk_buff *skb)
{
	unsigned int len = skb_transport_offset(skb) + sizeof(struct igmphdr);
	unsigned int transport_len = ip_transport_len(skb);
	struct sk_buff *skb_chk;

	if (!ip_mc_may_pull(skb, len))
		return -EINVAL;

	skb_chk = skb_checksum_trimmed(skb, transport_len,
				       ip_mc_validate_checksum);
	if (!skb_chk)
		return -EINVAL;

	if (skb_chk != skb)
		kfree_skb(skb_chk);

	return 0;
}