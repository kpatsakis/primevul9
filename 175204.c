static void skb_release_data(struct sk_buff *skb)
{
	if (!skb->cloned ||
	    !atomic_sub_return(skb->nohdr ? (1 << SKB_DATAREF_SHIFT) + 1 : 1,
			       &skb_shinfo(skb)->dataref)) {
		if (skb_shinfo(skb)->nr_frags) {
			int i;
			for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
				skb_frag_unref(skb, i);
		}

		/*
		 * If skb buf is from userspace, we need to notify the caller
		 * the lower device DMA has done;
		 */
		if (skb_shinfo(skb)->tx_flags & SKBTX_DEV_ZEROCOPY) {
			struct ubuf_info *uarg;

			uarg = skb_shinfo(skb)->destructor_arg;
			if (uarg->callback)
				uarg->callback(uarg, true);
		}

		if (skb_has_frag_list(skb))
			skb_drop_fraglist(skb);

		skb_free_head(skb);
	}
}