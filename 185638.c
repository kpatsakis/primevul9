mwifiex_is_ptr_processed(struct mwifiex_private *priv,
			 struct mwifiex_ra_list_tbl *ptr)
{
	struct sk_buff *skb;
	struct mwifiex_txinfo *tx_info;

	if (skb_queue_empty(&ptr->skb_head))
		return false;

	skb = skb_peek(&ptr->skb_head);

	tx_info = MWIFIEX_SKB_TXCB(skb);
	if (tx_info->flags & MWIFIEX_BUF_FLAG_REQUEUED_PKT)
		return true;

	return false;
}