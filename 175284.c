skb_zerocopy_headlen(const struct sk_buff *from)
{
	unsigned int hlen = 0;

	if (!from->head_frag ||
	    skb_headlen(from) < L1_CACHE_BYTES ||
	    skb_shinfo(from)->nr_frags >= MAX_SKB_FRAGS)
		hlen = skb_headlen(from);

	if (skb_has_frag_list(from))
		hlen = from->len;

	return hlen;
}