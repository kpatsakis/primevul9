struct sk_buff *__alloc_skb(unsigned int size, gfp_t gfp_mask,
			    int flags, int node)
{
	struct kmem_cache *cache;
	struct skb_shared_info *shinfo;
	struct sk_buff *skb;
	u8 *data;
	bool pfmemalloc;

	cache = (flags & SKB_ALLOC_FCLONE)
		? skbuff_fclone_cache : skbuff_head_cache;

	if (sk_memalloc_socks() && (flags & SKB_ALLOC_RX))
		gfp_mask |= __GFP_MEMALLOC;

	/* Get the HEAD */
	skb = kmem_cache_alloc_node(cache, gfp_mask & ~__GFP_DMA, node);
	if (!skb)
		goto out;
	prefetchw(skb);

	/* We do our best to align skb_shared_info on a separate cache
	 * line. It usually works because kmalloc(X > SMP_CACHE_BYTES) gives
	 * aligned memory blocks, unless SLUB/SLAB debug is enabled.
	 * Both skb->head and skb_shared_info are cache line aligned.
	 */
	size = SKB_DATA_ALIGN(size);
	size += SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	data = kmalloc_reserve(size, gfp_mask, node, &pfmemalloc);
	if (!data)
		goto nodata;
	/* kmalloc(size) might give us more room than requested.
	 * Put skb_shared_info exactly at the end of allocated zone,
	 * to allow max possible filling before reallocation.
	 */
	size = SKB_WITH_OVERHEAD(ksize(data));
	prefetchw(data + size);

	/*
	 * Only clear those fields we need to clear, not those that we will
	 * actually initialise below. Hence, don't put any more fields after
	 * the tail pointer in struct sk_buff!
	 */
	memset(skb, 0, offsetof(struct sk_buff, tail));
	/* Account for allocated memory : skb + skb->head */
	skb->truesize = SKB_TRUESIZE(size);
	skb->pfmemalloc = pfmemalloc;
	atomic_set(&skb->users, 1);
	skb->head = data;
	skb->data = data;
	skb_reset_tail_pointer(skb);
	skb->end = skb->tail + size;
	skb->mac_header = (typeof(skb->mac_header))~0U;
	skb->transport_header = (typeof(skb->transport_header))~0U;

	/* make sure we initialize shinfo sequentially */
	shinfo = skb_shinfo(skb);
	memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
	atomic_set(&shinfo->dataref, 1);
	kmemcheck_annotate_variable(shinfo->destructor_arg);

	if (flags & SKB_ALLOC_FCLONE) {
		struct sk_buff *child = skb + 1;
		atomic_t *fclone_ref = (atomic_t *) (child + 1);

		kmemcheck_annotate_bitfield(child, flags1);
		kmemcheck_annotate_bitfield(child, flags2);
		skb->fclone = SKB_FCLONE_ORIG;
		atomic_set(fclone_ref, 1);

		child->fclone = SKB_FCLONE_UNAVAILABLE;
		child->pfmemalloc = pfmemalloc;
	}
out:
	return skb;
nodata:
	kmem_cache_free(cache, skb);
	skb = NULL;
	goto out;
}