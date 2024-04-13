mwifiex_is_11n_aggragation_possible(struct mwifiex_private *priv,
				    struct mwifiex_ra_list_tbl *ptr,
				    int max_buf_size)
{
	int count = 0, total_size = 0;
	struct sk_buff *skb, *tmp;
	int max_amsdu_size;

	if (priv->bss_role == MWIFIEX_BSS_ROLE_UAP && priv->ap_11n_enabled &&
	    ptr->is_11n_enabled)
		max_amsdu_size = min_t(int, ptr->max_amsdu, max_buf_size);
	else
		max_amsdu_size = max_buf_size;

	skb_queue_walk_safe(&ptr->skb_head, skb, tmp) {
		total_size += skb->len;
		if (total_size >= max_amsdu_size)
			break;
		if (++count >= MIN_NUM_AMSDU)
			return true;
	}

	return false;
}