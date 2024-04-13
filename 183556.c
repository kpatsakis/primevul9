static int ip_mc_check_igmp_reportv3(struct sk_buff *skb)
{
	unsigned int len = skb_transport_offset(skb);

	len += sizeof(struct igmpv3_report);

	return ip_mc_may_pull(skb, len) ? 0 : -EINVAL;
}