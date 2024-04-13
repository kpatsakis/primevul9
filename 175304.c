void skb_split(struct sk_buff *skb, struct sk_buff *skb1, const u32 len)
{
	int pos = skb_headlen(skb);

	skb_shinfo(skb1)->tx_flags = skb_shinfo(skb)->tx_flags & SKBTX_SHARED_FRAG;
	if (len < pos)	/* Split line is inside header. */
		skb_split_inside_header(skb, skb1, len, pos);
	else		/* Second chunk has no header, nothing to copy. */
		skb_split_no_header(skb, skb1, len, pos);
}