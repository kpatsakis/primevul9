skb_zerocopy(struct sk_buff *to, struct sk_buff *from, int len, int hlen)
{
	int i, j = 0;
	int plen = 0; /* length of skb->head fragment */
	int ret;
	struct page *page;
	unsigned int offset;

	BUG_ON(!from->head_frag && !hlen);

	/* dont bother with small payloads */
	if (len <= skb_tailroom(to))
		return skb_copy_bits(from, 0, skb_put(to, len), len);

	if (hlen) {
		ret = skb_copy_bits(from, 0, skb_put(to, hlen), hlen);
		if (unlikely(ret))
			return ret;
		len -= hlen;
	} else {
		plen = min_t(int, skb_headlen(from), len);
		if (plen) {
			page = virt_to_head_page(from->head);
			offset = from->data - (unsigned char *)page_address(page);
			__skb_fill_page_desc(to, 0, page, offset, plen);
			get_page(page);
			j = 1;
			len -= plen;
		}
	}

	to->truesize += len + plen;
	to->len += len + plen;
	to->data_len += len + plen;

	if (unlikely(skb_orphan_frags(from, GFP_ATOMIC))) {
		skb_tx_error(from);
		return -ENOMEM;
	}

	for (i = 0; i < skb_shinfo(from)->nr_frags; i++) {
		if (!len)
			break;
		skb_shinfo(to)->frags[j] = skb_shinfo(from)->frags[i];
		skb_shinfo(to)->frags[j].size = min_t(int, skb_shinfo(to)->frags[j].size, len);
		len -= skb_shinfo(to)->frags[j].size;
		skb_frag_ref(to, j);
		j++;
	}
	skb_shinfo(to)->nr_frags = j;

	return 0;
}