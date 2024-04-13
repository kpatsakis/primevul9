static int udp6_ufo_send_check(struct sk_buff *skb)
{
	const struct ipv6hdr *ipv6h;
	struct udphdr *uh;

	/* UDP Tunnel offload on ipv6 is not yet supported. */
	if (skb->encapsulation)
		return -EINVAL;

	if (!pskb_may_pull(skb, sizeof(*uh)))
		return -EINVAL;

	ipv6h = ipv6_hdr(skb);
	uh = udp_hdr(skb);

	uh->check = ~csum_ipv6_magic(&ipv6h->saddr, &ipv6h->daddr, skb->len,
				     IPPROTO_UDP, 0);
	skb->csum_start = skb_transport_header(skb) - skb->head;
	skb->csum_offset = offsetof(struct udphdr, check);
	skb->ip_summed = CHECKSUM_PARTIAL;
	return 0;
}