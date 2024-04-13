static void kfree_skbmem(struct sk_buff *skb)
{
	struct sk_buff *other;
	atomic_t *fclone_ref;

	switch (skb->fclone) {
	case SKB_FCLONE_UNAVAILABLE:
		kmem_cache_free(skbuff_head_cache, skb);
		break;

	case SKB_FCLONE_ORIG:
		fclone_ref = (atomic_t *) (skb + 2);
		if (atomic_dec_and_test(fclone_ref))
			kmem_cache_free(skbuff_fclone_cache, skb);
		break;

	case SKB_FCLONE_CLONE:
		fclone_ref = (atomic_t *) (skb + 1);
		other = skb - 1;

		/* The clone portion is available for
		 * fast-cloning again.
		 */
		skb->fclone = SKB_FCLONE_UNAVAILABLE;

		if (atomic_dec_and_test(fclone_ref))
			kmem_cache_free(skbuff_fclone_cache, other);
		break;
	}
}