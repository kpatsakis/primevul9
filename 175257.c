bool skb_partial_csum_set(struct sk_buff *skb, u16 start, u16 off)
{
	if (unlikely(start > skb_headlen(skb)) ||
	    unlikely((int)start + off > skb_headlen(skb) - 2)) {
		net_warn_ratelimited("bad partial csum: csum=%u/%u len=%u\n",
				     start, off, skb_headlen(skb));
		return false;
	}
	skb->ip_summed = CHECKSUM_PARTIAL;
	skb->csum_start = skb_headroom(skb) + start;
	skb->csum_offset = off;
	skb_set_transport_header(skb, start);
	return true;
}