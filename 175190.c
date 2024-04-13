int skb_append_datato_frags(struct sock *sk, struct sk_buff *skb,
			int (*getfrag)(void *from, char *to, int offset,
					int len, int odd, struct sk_buff *skb),
			void *from, int length)
{
	int frg_cnt = skb_shinfo(skb)->nr_frags;
	int copy;
	int offset = 0;
	int ret;
	struct page_frag *pfrag = &current->task_frag;

	do {
		/* Return error if we don't have space for new frag */
		if (frg_cnt >= MAX_SKB_FRAGS)
			return -EMSGSIZE;

		if (!sk_page_frag_refill(sk, pfrag))
			return -ENOMEM;

		/* copy the user data to page */
		copy = min_t(int, length, pfrag->size - pfrag->offset);

		ret = getfrag(from, page_address(pfrag->page) + pfrag->offset,
			      offset, copy, 0, skb);
		if (ret < 0)
			return -EFAULT;

		/* copy was successful so update the size parameters */
		skb_fill_page_desc(skb, frg_cnt, pfrag->page, pfrag->offset,
				   copy);
		frg_cnt++;
		pfrag->offset += copy;
		get_page(pfrag->page);

		skb->truesize += copy;
		atomic_add(copy, &sk->sk_wmem_alloc);
		skb->len += copy;
		skb->data_len += copy;
		offset += copy;
		length -= copy;

	} while (length > 0);

	return 0;
}