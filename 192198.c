static inline __sum16 skb_checksum_complete(struct sk_buff *skb)
{
	return skb_csum_unnecessary(skb) ?
	       0 : __skb_checksum_complete(skb);
}