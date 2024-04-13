int skb_copy_ubufs(struct sk_buff *skb, gfp_t gfp_mask)
{
	int i;
	int num_frags = skb_shinfo(skb)->nr_frags;
	struct page *page, *head = NULL;
	struct ubuf_info *uarg = skb_shinfo(skb)->destructor_arg;

	for (i = 0; i < num_frags; i++) {
		u8 *vaddr;
		skb_frag_t *f = &skb_shinfo(skb)->frags[i];

		page = alloc_page(gfp_mask);
		if (!page) {
			while (head) {
				struct page *next = (struct page *)page_private(head);
				put_page(head);
				head = next;
			}
			return -ENOMEM;
		}
		vaddr = kmap_atomic(skb_frag_page(f));
		memcpy(page_address(page),
		       vaddr + f->page_offset, skb_frag_size(f));
		kunmap_atomic(vaddr);
		set_page_private(page, (unsigned long)head);
		head = page;
	}

	/* skb frags release userspace buffers */
	for (i = 0; i < num_frags; i++)
		skb_frag_unref(skb, i);

	uarg->callback(uarg, false);

	/* skb frags point to kernel buffers */
	for (i = num_frags - 1; i >= 0; i--) {
		__skb_fill_page_desc(skb, i, head, 0,
				     skb_shinfo(skb)->frags[i].size);
		head = (struct page *)page_private(head);
	}

	skb_shinfo(skb)->tx_flags &= ~SKBTX_DEV_ZEROCOPY;
	return 0;
}