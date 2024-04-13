struct sk_buff *__netdev_alloc_skb(struct net_device *dev,
				   unsigned int length, gfp_t gfp_mask)
{
	struct sk_buff *skb = NULL;
	unsigned int fragsz = SKB_DATA_ALIGN(length + NET_SKB_PAD) +
			      SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	if (fragsz <= PAGE_SIZE && !(gfp_mask & (__GFP_WAIT | GFP_DMA))) {
		void *data;

		if (sk_memalloc_socks())
			gfp_mask |= __GFP_MEMALLOC;

		data = __netdev_alloc_frag(fragsz, gfp_mask);

		if (likely(data)) {
			skb = build_skb(data, fragsz);
			if (unlikely(!skb))
				put_page(virt_to_head_page(data));
		}
	} else {
		skb = __alloc_skb(length + NET_SKB_PAD, gfp_mask,
				  SKB_ALLOC_RX, NUMA_NO_NODE);
	}
	if (likely(skb)) {
		skb_reserve(skb, NET_SKB_PAD);
		skb->dev = dev;
	}
	return skb;
}