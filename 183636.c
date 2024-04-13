static int ip_mc_check_iphdr(struct sk_buff *skb)
{
	const struct iphdr *iph;
	unsigned int len;
	unsigned int offset = skb_network_offset(skb) + sizeof(*iph);

	if (!pskb_may_pull(skb, offset))
		return -EINVAL;

	iph = ip_hdr(skb);

	if (iph->version != 4 || ip_hdrlen(skb) < sizeof(*iph))
		return -EINVAL;

	offset += ip_hdrlen(skb) - sizeof(*iph);

	if (!pskb_may_pull(skb, offset))
		return -EINVAL;

	iph = ip_hdr(skb);

	if (unlikely(ip_fast_csum((u8 *)iph, iph->ihl)))
		return -EINVAL;

	len = skb_network_offset(skb) + ntohs(iph->tot_len);
	if (skb->len < len || len < offset)
		return -EINVAL;

	skb_set_transport_header(skb, offset);

	return 0;
}