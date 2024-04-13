int ___pskb_trim(struct sk_buff *skb, unsigned int len)
{
	struct sk_buff **fragp;
	struct sk_buff *frag;
	int offset = skb_headlen(skb);
	int nfrags = skb_shinfo(skb)->nr_frags;
	int i;
	int err;

	if (skb_cloned(skb) &&
	    unlikely((err = pskb_expand_head(skb, 0, 0, GFP_ATOMIC))))
		return err;

	i = 0;
	if (offset >= len)
		goto drop_pages;

	for (; i < nfrags; i++) {
		int end = offset + skb_frag_size(&skb_shinfo(skb)->frags[i]);

		if (end < len) {
			offset = end;
			continue;
		}

		skb_frag_size_set(&skb_shinfo(skb)->frags[i++], len - offset);

drop_pages:
		skb_shinfo(skb)->nr_frags = i;

		for (; i < nfrags; i++)
			skb_frag_unref(skb, i);

		if (skb_has_frag_list(skb))
			skb_drop_fraglist(skb);
		goto done;
	}

	for (fragp = &skb_shinfo(skb)->frag_list; (frag = *fragp);
	     fragp = &frag->next) {
		int end = offset + frag->len;

		if (skb_shared(frag)) {
			struct sk_buff *nfrag;

			nfrag = skb_clone(frag, GFP_ATOMIC);
			if (unlikely(!nfrag))
				return -ENOMEM;

			nfrag->next = frag->next;
			consume_skb(frag);
			frag = nfrag;
			*fragp = frag;
		}

		if (end < len) {
			offset = end;
			continue;
		}

		if (end > len &&
		    unlikely((err = pskb_trim(frag, len - offset))))
			return err;

		if (frag->next)
			skb_drop_list(&frag->next);
		break;
	}

done:
	if (len > skb_headlen(skb)) {
		skb->data_len -= skb->len - len;
		skb->len       = len;
	} else {
		skb->len       = len;
		skb->data_len  = 0;
		skb_set_tail_pointer(skb, len);
	}

	return 0;
}