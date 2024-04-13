int skb_shift(struct sk_buff *tgt, struct sk_buff *skb, int shiftlen)
{
	int from, to, merge, todo;
	struct skb_frag_struct *fragfrom, *fragto;

	BUG_ON(shiftlen > skb->len);
	BUG_ON(skb_headlen(skb));	/* Would corrupt stream */

	todo = shiftlen;
	from = 0;
	to = skb_shinfo(tgt)->nr_frags;
	fragfrom = &skb_shinfo(skb)->frags[from];

	/* Actual merge is delayed until the point when we know we can
	 * commit all, so that we don't have to undo partial changes
	 */
	if (!to ||
	    !skb_can_coalesce(tgt, to, skb_frag_page(fragfrom),
			      fragfrom->page_offset)) {
		merge = -1;
	} else {
		merge = to - 1;

		todo -= skb_frag_size(fragfrom);
		if (todo < 0) {
			if (skb_prepare_for_shift(skb) ||
			    skb_prepare_for_shift(tgt))
				return 0;

			/* All previous frag pointers might be stale! */
			fragfrom = &skb_shinfo(skb)->frags[from];
			fragto = &skb_shinfo(tgt)->frags[merge];

			skb_frag_size_add(fragto, shiftlen);
			skb_frag_size_sub(fragfrom, shiftlen);
			fragfrom->page_offset += shiftlen;

			goto onlymerged;
		}

		from++;
	}

	/* Skip full, not-fitting skb to avoid expensive operations */
	if ((shiftlen == skb->len) &&
	    (skb_shinfo(skb)->nr_frags - from) > (MAX_SKB_FRAGS - to))
		return 0;

	if (skb_prepare_for_shift(skb) || skb_prepare_for_shift(tgt))
		return 0;

	while ((todo > 0) && (from < skb_shinfo(skb)->nr_frags)) {
		if (to == MAX_SKB_FRAGS)
			return 0;

		fragfrom = &skb_shinfo(skb)->frags[from];
		fragto = &skb_shinfo(tgt)->frags[to];

		if (todo >= skb_frag_size(fragfrom)) {
			*fragto = *fragfrom;
			todo -= skb_frag_size(fragfrom);
			from++;
			to++;

		} else {
			__skb_frag_ref(fragfrom);
			fragto->page = fragfrom->page;
			fragto->page_offset = fragfrom->page_offset;
			skb_frag_size_set(fragto, todo);

			fragfrom->page_offset += todo;
			skb_frag_size_sub(fragfrom, todo);
			todo = 0;

			to++;
			break;
		}
	}

	/* Ready to "commit" this state change to tgt */
	skb_shinfo(tgt)->nr_frags = to;

	if (merge >= 0) {
		fragfrom = &skb_shinfo(skb)->frags[0];
		fragto = &skb_shinfo(tgt)->frags[merge];

		skb_frag_size_add(fragto, skb_frag_size(fragfrom));
		__skb_frag_unref(fragfrom);
	}

	/* Reposition in the original skb */
	to = 0;
	while (from < skb_shinfo(skb)->nr_frags)
		skb_shinfo(skb)->frags[to++] = skb_shinfo(skb)->frags[from++];
	skb_shinfo(skb)->nr_frags = to;

	BUG_ON(todo > 0 && !skb_shinfo(skb)->nr_frags);

onlymerged:
	/* Most likely the tgt won't ever need its checksum anymore, skb on
	 * the other hand might need it if it needs to be resent
	 */
	tgt->ip_summed = CHECKSUM_PARTIAL;
	skb->ip_summed = CHECKSUM_PARTIAL;

	/* Yak, is it really working this way? Some helper please? */
	skb->len -= shiftlen;
	skb->data_len -= shiftlen;
	skb->truesize -= shiftlen;
	tgt->len += shiftlen;
	tgt->data_len += shiftlen;
	tgt->truesize += shiftlen;

	return shiftlen;
}