mwifiex_wmm_del_peer_ra_list(struct mwifiex_private *priv, const u8 *ra_addr)
{
	struct mwifiex_ra_list_tbl *ra_list;
	int i;

	spin_lock_bh(&priv->wmm.ra_list_spinlock);

	for (i = 0; i < MAX_NUM_TID; ++i) {
		ra_list = mwifiex_wmm_get_ralist_node(priv, i, ra_addr);

		if (!ra_list)
			continue;
		mwifiex_wmm_del_pkts_in_ralist_node(priv, ra_list);
		if (ra_list->tx_paused)
			priv->wmm.pkts_paused[i] -= ra_list->total_pkt_count;
		else
			atomic_sub(ra_list->total_pkt_count,
				   &priv->wmm.tx_pkts_queued);
		list_del(&ra_list->list);
		kfree(ra_list);
	}
	spin_unlock_bh(&priv->wmm.ra_list_spinlock);
}