void skb_add_rx_frag(struct sk_buff *skb, int i, struct page *page, int off,
		     int size, unsigned int truesize)
{
	skb_fill_page_desc(skb, i, page, off, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
}