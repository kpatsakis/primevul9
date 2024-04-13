mwifiex_wmm_del_pkts_in_ralist_node(struct mwifiex_private *priv,
				    struct mwifiex_ra_list_tbl *ra_list)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct sk_buff *skb, *tmp;

	skb_queue_walk_safe(&ra_list->skb_head, skb, tmp) {
		skb_unlink(skb, &ra_list->skb_head);
		mwifiex_write_data_complete(adapter, skb, 0, -1);
	}
}