mwifiex_wmm_del_pkts_in_ralist(struct mwifiex_private *priv,
			       struct list_head *ra_list_head)
{
	struct mwifiex_ra_list_tbl *ra_list;

	list_for_each_entry(ra_list, ra_list_head, list)
		mwifiex_wmm_del_pkts_in_ralist_node(priv, ra_list);
}