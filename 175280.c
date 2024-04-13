int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail,
		     gfp_t gfp_mask)
{
	int i;
	u8 *data;
	int size = nhead + skb_end_offset(skb) + ntail;
	long off;

	BUG_ON(nhead < 0);

	if (skb_shared(skb))
		BUG();

	size = SKB_DATA_ALIGN(size);

	if (skb_pfmemalloc(skb))
		gfp_mask |= __GFP_MEMALLOC;
	data = kmalloc_reserve(size + SKB_DATA_ALIGN(sizeof(struct skb_shared_info)),
			       gfp_mask, NUMA_NO_NODE, NULL);
	if (!data)
		goto nodata;
	size = SKB_WITH_OVERHEAD(ksize(data));

	/* Copy only real data... and, alas, header. This should be
	 * optimized for the cases when header is void.
	 */
	memcpy(data + nhead, skb->head, skb_tail_pointer(skb) - skb->head);

	memcpy((struct skb_shared_info *)(data + size),
	       skb_shinfo(skb),
	       offsetof(struct skb_shared_info, frags[skb_shinfo(skb)->nr_frags]));

	/*
	 * if shinfo is shared we must drop the old head gracefully, but if it
	 * is not we can just drop the old head and let the existing refcount
	 * be since all we did is relocate the values
	 */
	if (skb_cloned(skb)) {
		/* copy this zero copy skb frags */
		if (skb_orphan_frags(skb, gfp_mask))
			goto nofrags;
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
			skb_frag_ref(skb, i);

		if (skb_has_frag_list(skb))
			skb_clone_fraglist(skb);

		skb_release_data(skb);
	} else {
		skb_free_head(skb);
	}
	off = (data + nhead) - skb->head;

	skb->head     = data;
	skb->head_frag = 0;
	skb->data    += off;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	skb->end      = size;
	off           = nhead;
#else
	skb->end      = skb->head + size;
#endif
	skb->tail	      += off;
	skb_headers_offset_update(skb, nhead);
	skb->cloned   = 0;
	skb->hdr_len  = 0;
	skb->nohdr    = 0;
	atomic_set(&skb_shinfo(skb)->dataref, 1);
	return 0;

nofrags:
	kfree(data);
nodata:
	return -ENOMEM;
}