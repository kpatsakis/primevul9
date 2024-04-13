mwifiex_wmm_get_ralist_node(struct mwifiex_private *priv, u8 tid,
			    const u8 *ra_addr)
{
	struct mwifiex_ra_list_tbl *ra_list;

	list_for_each_entry(ra_list, &priv->wmm.tid_tbl_ptr[tid].ra_list,
			    list) {
		if (!memcmp(ra_list->ra, ra_addr, ETH_ALEN))
			return ra_list;
	}

	return NULL;
}