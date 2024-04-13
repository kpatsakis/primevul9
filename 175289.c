struct sk_buff *build_skb(void *data, unsigned int frag_size)
{
	struct skb_shared_info *shinfo;
	struct sk_buff *skb;
	unsigned int size = frag_size ? : ksize(data);

	skb = kmem_cache_alloc(skbuff_head_cache, GFP_ATOMIC);
	if (!skb)
		return NULL;

	size -= SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	memset(skb, 0, offsetof(struct sk_buff, tail));
	skb->truesize = SKB_TRUESIZE(size);
	skb->head_frag = frag_size != 0;
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

	return skb;
}