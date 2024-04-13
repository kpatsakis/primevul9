static void mwifiex_wmm_delete_all_ralist(struct mwifiex_private *priv)
{
	struct mwifiex_ra_list_tbl *ra_list, *tmp_node;
	int i;

	for (i = 0; i < MAX_NUM_TID; ++i) {
		mwifiex_dbg(priv->adapter, INFO,
			    "info: ra_list: freeing buf for tid %d\n", i);
		list_for_each_entry_safe(ra_list, tmp_node,
					 &priv->wmm.tid_tbl_ptr[i].ra_list,
					 list) {
			list_del(&ra_list->list);
			kfree(ra_list);
		}

		INIT_LIST_HEAD(&priv->wmm.tid_tbl_ptr[i].ra_list);
	}
}