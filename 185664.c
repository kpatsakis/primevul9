mwifiex_wmm_allocate_ralist_node(struct mwifiex_adapter *adapter, const u8 *ra)
{
	struct mwifiex_ra_list_tbl *ra_list;

	ra_list = kzalloc(sizeof(struct mwifiex_ra_list_tbl), GFP_ATOMIC);
	if (!ra_list)
		return NULL;

	INIT_LIST_HEAD(&ra_list->list);
	skb_queue_head_init(&ra_list->skb_head);

	memcpy(ra_list->ra, ra, ETH_ALEN);

	ra_list->total_pkt_count = 0;

	mwifiex_dbg(adapter, INFO, "info: allocated ra_list %p\n", ra_list);

	return ra_list;
}