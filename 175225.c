static void skb_headers_offset_update(struct sk_buff *skb, int off)
{
	/* Only adjust this if it actually is csum_start rather than csum */
	if (skb->ip_summed == CHECKSUM_PARTIAL)
		skb->csum_start += off;
	/* {transport,network,mac}_header and tail are relative to skb->head */
	skb->transport_header += off;
	skb->network_header   += off;
	if (skb_mac_header_was_set(skb))
		skb->mac_header += off;
	skb->inner_transport_header += off;
	skb->inner_network_header += off;
	skb->inner_mac_header += off;
}