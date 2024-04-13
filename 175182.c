unsigned int skb_gso_transport_seglen(const struct sk_buff *skb)
{
	const struct skb_shared_info *shinfo = skb_shinfo(skb);
	unsigned int hdr_len;

	if (likely(shinfo->gso_type & (SKB_GSO_TCPV4 | SKB_GSO_TCPV6)))
		hdr_len = tcp_hdrlen(skb);
	else
		hdr_len = sizeof(struct udphdr);
	return hdr_len + shinfo->gso_size;
}