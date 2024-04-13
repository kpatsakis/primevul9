static void mwifiex_wmm_cleanup_queues(struct mwifiex_private *priv)
{
	int i;

	for (i = 0; i < MAX_NUM_TID; i++)
		mwifiex_wmm_del_pkts_in_ralist(priv, &priv->wmm.tid_tbl_ptr[i].
								       ra_list);

	atomic_set(&priv->wmm.tx_pkts_queued, 0);
	atomic_set(&priv->wmm.highest_queued_prio, HIGH_PRIO_TID);
}