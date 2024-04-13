int ip_mc_check_igmp(struct sk_buff *skb)
{
	int ret = ip_mc_check_iphdr(skb);

	if (ret < 0)
		return ret;

	if (ip_hdr(skb)->protocol != IPPROTO_IGMP)
		return -ENOMSG;

	ret = ip_mc_check_igmp_csum(skb);
	if (ret < 0)
		return ret;

	return ip_mc_check_igmp_msg(skb);
}