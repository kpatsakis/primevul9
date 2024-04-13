static inline void skb_split_no_header(struct sk_buff *skb,
				       struct sk_buff* skb1,
				       const u32 len, int pos)
{
	int i, k = 0;
	const int nfrags = skb_shinfo(skb)->nr_frags;

	skb_shinfo(skb)->nr_frags = 0;
	skb1->len		  = skb1->data_len = skb->len - len;
	skb->len		  = len;
	skb->data_len		  = len - pos;

	for (i = 0; i < nfrags; i++) {
		int size = skb_frag_size(&skb_shinfo(skb)->frags[i]);

		if (pos + size > len) {
			skb_shinfo(skb1)->frags[k] = skb_shinfo(skb)->frags[i];

			if (pos < len) {
				/* Split frag.
				 * We have two variants in this case:
				 * 1. Move all the frag to the second
				 *    part, if it is possible. F.e.
				 *    this approach is mandatory for TUX,
				 *    where splitting is expensive.
				 * 2. Split is accurately. We make this.
				 */
				skb_frag_ref(skb, i);
				skb_shinfo(skb1)->frags[0].page_offset += len - pos;
				skb_frag_size_sub(&skb_shinfo(skb1)->frags[0], len - pos);
				skb_frag_size_set(&skb_shinfo(skb)->frags[i], len - pos);
				skb_shinfo(skb)->nr_frags++;
			}
			k++;
		} else
			skb_shinfo(skb)->nr_frags++;
		pos += size;
	}
	skb_shinfo(skb1)->nr_frags = k;
}