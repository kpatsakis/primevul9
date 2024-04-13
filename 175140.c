void kfree_skb_partial(struct sk_buff *skb, bool head_stolen)
{
	if (head_stolen) {
		skb_release_head_state(skb);
		kmem_cache_free(skbuff_head_cache, skb);
	} else {
		__kfree_skb(skb);
	}
}