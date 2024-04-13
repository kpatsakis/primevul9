unsigned int skb_seq_read(unsigned int consumed, const u8 **data,
			  struct skb_seq_state *st)
{
	unsigned int block_limit, abs_offset = consumed + st->lower_offset;
	skb_frag_t *frag;

	if (unlikely(abs_offset >= st->upper_offset)) {
		if (st->frag_data) {
			kunmap_atomic(st->frag_data);
			st->frag_data = NULL;
		}
		return 0;
	}

next_skb:
	block_limit = skb_headlen(st->cur_skb) + st->stepped_offset;

	if (abs_offset < block_limit && !st->frag_data) {
		*data = st->cur_skb->data + (abs_offset - st->stepped_offset);
		return block_limit - abs_offset;
	}

	if (st->frag_idx == 0 && !st->frag_data)
		st->stepped_offset += skb_headlen(st->cur_skb);

	while (st->frag_idx < skb_shinfo(st->cur_skb)->nr_frags) {
		frag = &skb_shinfo(st->cur_skb)->frags[st->frag_idx];
		block_limit = skb_frag_size(frag) + st->stepped_offset;

		if (abs_offset < block_limit) {
			if (!st->frag_data)
				st->frag_data = kmap_atomic(skb_frag_page(frag));

			*data = (u8 *) st->frag_data + frag->page_offset +
				(abs_offset - st->stepped_offset);

			return block_limit - abs_offset;
		}

		if (st->frag_data) {
			kunmap_atomic(st->frag_data);
			st->frag_data = NULL;
		}

		st->frag_idx++;
		st->stepped_offset += skb_frag_size(frag);
	}

	if (st->frag_data) {
		kunmap_atomic(st->frag_data);
		st->frag_data = NULL;
	}

	if (st->root_skb == st->cur_skb && skb_has_frag_list(st->root_skb)) {
		st->cur_skb = skb_shinfo(st->root_skb)->frag_list;
		st->frag_idx = 0;
		goto next_skb;
	} else if (st->cur_skb->next) {
		st->cur_skb = st->cur_skb->next;
		st->frag_idx = 0;
		goto next_skb;
	}

	return 0;
}