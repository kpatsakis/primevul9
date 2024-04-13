void mwifiex_ralist_add(struct mwifiex_private *priv, const u8 *ra)
{
	int i;
	struct mwifiex_ra_list_tbl *ra_list;
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_sta_node *node;


	for (i = 0; i < MAX_NUM_TID; ++i) {
		ra_list = mwifiex_wmm_allocate_ralist_node(adapter, ra);
		mwifiex_dbg(adapter, INFO,
			    "info: created ra_list %p\n", ra_list);

		if (!ra_list)
			break;

		ra_list->is_11n_enabled = 0;
		ra_list->tdls_link = false;
		ra_list->ba_status = BA_SETUP_NONE;
		ra_list->amsdu_in_ampdu = false;
		if (!mwifiex_queuing_ra_based(priv)) {
			if (mwifiex_is_tdls_link_setup
				(mwifiex_get_tdls_link_status(priv, ra))) {
				ra_list->tdls_link = true;
				ra_list->is_11n_enabled =
					mwifiex_tdls_peer_11n_enabled(priv, ra);
			} else {
				ra_list->is_11n_enabled = IS_11N_ENABLED(priv);
			}
		} else {
			spin_lock_bh(&priv->sta_list_spinlock);
			node = mwifiex_get_sta_entry(priv, ra);
			if (node)
				ra_list->tx_paused = node->tx_pause;
			ra_list->is_11n_enabled =
				      mwifiex_is_sta_11n_enabled(priv, node);
			if (ra_list->is_11n_enabled)
				ra_list->max_amsdu = node->max_amsdu;
			spin_unlock_bh(&priv->sta_list_spinlock);
		}

		mwifiex_dbg(adapter, DATA, "data: ralist %p: is_11n_enabled=%d\n",
			    ra_list, ra_list->is_11n_enabled);

		if (ra_list->is_11n_enabled) {
			ra_list->ba_pkt_count = 0;
			ra_list->ba_packet_thr =
					      mwifiex_get_random_ba_threshold();
		}
		list_add_tail(&ra_list->list,
			      &priv->wmm.tid_tbl_ptr[i].ra_list);
	}
}