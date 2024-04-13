int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len)
{
	int start = skb_headlen(skb);
	struct sk_buff *frag_iter;
	int i, copy;

	if (offset > (int)skb->len - len)
		goto fault;

	/* Copy header. */
	if ((copy = start - offset) > 0) {
		if (copy > len)
			copy = len;
		skb_copy_from_linear_data_offset(skb, offset, to, copy);
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		to     += copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;
		skb_frag_t *f = &skb_shinfo(skb)->frags[i];

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(f);
		if ((copy = end - offset) > 0) {
			u8 *vaddr;

			if (copy > len)
				copy = len;

			vaddr = kmap_atomic(skb_frag_page(f));
			memcpy(to,
			       vaddr + f->page_offset + offset - start,
			       copy);
			kunmap_atomic(vaddr);

			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			to     += copy;
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
			if (skb_copy_bits(frag_iter, offset - start, to, copy))
				goto fault;
			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			to     += copy;
		}
		start = end;
	}

	if (!len)
		return 0;

fault:
	return -EFAULT;
}