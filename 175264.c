__wsum __skb_checksum(const struct sk_buff *skb, int offset, int len,
		      __wsum csum, const struct skb_checksum_ops *ops)
{
	int start = skb_headlen(skb);
	int i, copy = start - offset;
	struct sk_buff *frag_iter;
	int pos = 0;

	/* Checksum header. */
	if (copy > 0) {
		if (copy > len)
			copy = len;
		csum = ops->update(skb->data + offset, copy, csum);
		if ((len -= copy) == 0)
			return csum;
		offset += copy;
		pos	= copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(frag);
		if ((copy = end - offset) > 0) {
			__wsum csum2;
			u8 *vaddr;

			if (copy > len)
				copy = len;
			vaddr = kmap_atomic(skb_frag_page(frag));
			csum2 = ops->update(vaddr + frag->page_offset +
					    offset - start, copy, 0);
			kunmap_atomic(vaddr);
			csum = ops->combine(csum, csum2, pos, copy);
			if (!(len -= copy))
				return csum;
			offset += copy;
			pos    += copy;
		}
		start = end;
	}

	skb_walk_frags(skb, frag_iter) {
		int end;

		WARN_ON(start > offset + len);

		end = start + frag_iter->len;
		if ((copy = end - offset) > 0) {
			__wsum csum2;
			if (copy > len)
				copy = len;
			csum2 = __skb_checksum(frag_iter, offset - start,
					       copy, 0, ops);
			csum = ops->combine(csum, csum2, pos, copy);
			if ((len -= copy) == 0)
				return csum;
			offset += copy;
			pos    += copy;
		}
		start = end;
	}
	BUG_ON(len);

	return csum;
}