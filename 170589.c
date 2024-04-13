static unsigned long atalk_sum_skb(const struct sk_buff *skb, int offset,
				   int len, unsigned long sum)
{
	int start = skb_headlen(skb);
	struct sk_buff *frag_iter;
	int i, copy;

	/* checksum stuff in header space */
	if ((copy = start - offset) > 0) {
		if (copy > len)
			copy = len;
		sum = atalk_sum_partial(skb->data + offset, copy, sum);
		if ((len -= copy) == 0)
			return sum;

		offset += copy;
	}

	/* checksum stuff in frags */
	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		WARN_ON(start > offset + len);

		end = start + skb_frag_size(frag);
		if ((copy = end - offset) > 0) {
			u8 *vaddr;

			if (copy > len)
				copy = len;
			vaddr = kmap_atomic(skb_frag_page(frag));
			sum = atalk_sum_partial(vaddr + frag->page_offset +
						  offset - start, copy, sum);
			kunmap_atomic(vaddr);

			if (!(len -= copy))
				return sum;
			offset += copy;
		}
		start = end;
	}

	skb_walk_frags(skb, frag_iter) {
		int end;

		WARN_ON(start > offset + len);

		end = start + frag_iter->len;
		if ((copy = end - offset) > 0) {
			if (copy > len)
				copy = len;
			sum = atalk_sum_skb(frag_iter, offset - start,
					    copy, sum);
			if ((len -= copy) == 0)
				return sum;
			offset += copy;
		}
		start = end;
	}

	BUG_ON(len > 0);

	return sum;
}