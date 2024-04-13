mwifiex_wmm_get_queue_raptr(struct mwifiex_private *priv, u8 tid,
			    const u8 *ra_addr)
{
	struct mwifiex_ra_list_tbl *ra_list;

	ra_list = mwifiex_wmm_get_ralist_node(priv, tid, ra_addr);
	if (ra_list)
		return ra_list;
	mwifiex_ralist_add(priv, ra_addr);

	return mwifiex_wmm_get_ralist_node(priv, tid, ra_addr);
}