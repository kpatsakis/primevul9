struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t gfp_mask)
{
	int headerlen = skb_headroom(skb);
	unsigned int size = skb_end_offset(skb) + skb->data_len;
	struct sk_buff *n = __alloc_skb(size, gfp_mask,
					skb_alloc_rx_flag(skb), NUMA_NO_NODE);

	if (!n)
		return NULL;

	/* Set the data pointer */
	skb_reserve(n, headerlen);
	/* Set the tail pointer and length */
	skb_put(n, skb->len);

	if (skb_copy_bits(skb, -headerlen, n->head, headerlen + skb->len))
		BUG();

	copy_skb_header(n, skb);
	return n;
}