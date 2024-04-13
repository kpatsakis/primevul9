bool skb_try_coalesce(struct sk_buff *to, struct sk_buff *from,
		      bool *fragstolen, int *delta_truesize)
{
	int i, delta, len = from->len;

	*fragstolen = false;

	if (skb_cloned(to))
		return false;

	if (len <= skb_tailroom(to)) {
		BUG_ON(skb_copy_bits(from, 0, skb_put(to, len), len));
		*delta_truesize = 0;
		return true;
	}

	if (skb_has_frag_list(to) || skb_has_frag_list(from))
		return false;

	if (skb_headlen(from) != 0) {
		struct page *page;
		unsigned int offset;

		if (skb_shinfo(to)->nr_frags +
		    skb_shinfo(from)->nr_frags >= MAX_SKB_FRAGS)
			return false;

		if (skb_head_is_locked(from))
			return false;

		delta = from->truesize - SKB_DATA_ALIGN(sizeof(struct sk_buff));

		page = virt_to_head_page(from->head);
		offset = from->data - (unsigned char *)page_address(page);

		skb_fill_page_desc(to, skb_shinfo(to)->nr_frags,
				   page, offset, skb_headlen(from));
		*fragstolen = true;
	} else {
		if (skb_shinfo(to)->nr_frags +
		    skb_shinfo(from)->nr_frags > MAX_SKB_FRAGS)
			return false;

		delta = from->truesize - SKB_TRUESIZE(skb_end_offset(from));
	}

	WARN_ON_ONCE(delta < len);

	memcpy(skb_shinfo(to)->frags + skb_shinfo(to)->nr_frags,
	       skb_shinfo(from)->frags,
	       skb_shinfo(from)->nr_frags * sizeof(skb_frag_t));
	skb_shinfo(to)->nr_frags += skb_shinfo(from)->nr_frags;

	if (!skb_cloned(from))
		skb_shinfo(from)->nr_frags = 0;

	/* if the skb is not cloned this does nothing
	 * since we set nr_frags to 0.
	 */
	for (i = 0; i < skb_shinfo(from)->nr_frags; i++)
		skb_frag_ref(from, i);

	to->truesize += delta;
	to->len += len;
	to->data_len += len;

	*delta_truesize = delta;
	return true;
}