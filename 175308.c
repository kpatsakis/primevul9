struct sk_buff *skb_clone(struct sk_buff *skb, gfp_t gfp_mask)
{
	struct sk_buff *n;

	if (skb_orphan_frags(skb, gfp_mask))
		return NULL;

	n = skb + 1;
	if (skb->fclone == SKB_FCLONE_ORIG &&
	    n->fclone == SKB_FCLONE_UNAVAILABLE) {
		atomic_t *fclone_ref = (atomic_t *) (n + 1);
		n->fclone = SKB_FCLONE_CLONE;
		atomic_inc(fclone_ref);
	} else {
		if (skb_pfmemalloc(skb))
			gfp_mask |= __GFP_MEMALLOC;

		n = kmem_cache_alloc(skbuff_head_cache, gfp_mask);
		if (!n)
			return NULL;

		kmemcheck_annotate_bitfield(n, flags1);
		kmemcheck_annotate_bitfield(n, flags2);
		n->fclone = SKB_FCLONE_UNAVAILABLE;
	}

	return __skb_clone(n, skb);
}