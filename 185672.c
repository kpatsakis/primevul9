mwifiex_is_ralist_valid(struct mwifiex_private *priv,
			struct mwifiex_ra_list_tbl *ra_list, int ptr_index)
{
	struct mwifiex_ra_list_tbl *rlist;

	list_for_each_entry(rlist, &priv->wmm.tid_tbl_ptr[ptr_index].ra_list,
			    list) {
		if (rlist == ra_list)
			return true;
	}

	return false;
}