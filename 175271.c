void skb_coalesce_rx_frag(struct sk_buff *skb, int i, int size,
			  unsigned int truesize)
{
	skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

	skb_frag_size_add(frag, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
}