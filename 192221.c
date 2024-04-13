static inline int skb_checksum_start_offset(const struct sk_buff *skb)
{
	return skb->csum_start - skb_headroom(skb);
}