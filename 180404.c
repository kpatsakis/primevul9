void cipso_v4_error(struct sk_buff *skb, int error, u32 gateway)
{
	if (ip_hdr(skb)->protocol == IPPROTO_ICMP || error != -EACCES)
		return;

	if (gateway)
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_ANO, 0);
	else
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_HOST_ANO, 0);
}