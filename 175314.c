__skb_to_sgvec(struct sk_buff *skb, struct scatterlist *sg, int offset, int len)
{
	int start = skb_headlen(skb);
	int i, copy = start - offset;
	struct sk_buff *frag_iter;
	int elt = 0;

	if (copy > 0) {
		if (copy > len)
			copy = len;
		sg_set_buf(sg, skb->data + offset, copy);
		elt++;
		if ((len -= copy) == 0)
			return elt;
		offset += copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(&skb_shinfo(skb)->frags[i]);
		if ((copy = end - offset) > 0) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			if (copy > len)
				copy = len;
			sg_set_page(&sg[elt], skb_frag_page(frag), copy,
					frag->page_offset+offset-start);
			elt++;
			if (!(len -= copy))
				return elt;
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
			elt += __skb_to_sgvec(frag_iter, sg+elt, offset - start,
					      copy);
			if ((len -= copy) == 0)
				return elt;
			offset += copy;
		}
		start = end;
	}
	BUG_ON(len);
	return elt;
}