int skb_store_bits(struct sk_buff *skb, int offset, const void *from, int len)
{
	int start = skb_headlen(skb);
	struct sk_buff *frag_iter;
	int i, copy;

	if (offset > (int)skb->len - len)
		goto fault;

	if ((copy = start - offset) > 0) {
		if (copy > len)
			copy = len;
		skb_copy_to_linear_data_offset(skb, offset, from, copy);
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		from += copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		int end;

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(frag);
		if ((copy = end - offset) > 0) {
			u8 *vaddr;

			if (copy > len)
				copy = len;

			vaddr = kmap_atomic(skb_frag_page(frag));
			memcpy(vaddr + frag->page_offset + offset - start,
			       from, copy);
			kunmap_atomic(vaddr);

			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			from += copy;
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
			if (skb_store_bits(frag_iter, offset - start,
					   from, copy))
				goto fault;
			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			from += copy;
		}
		start = end;
	}
	if (!len)
		return 0;

fault:
	return -EFAULT;
}