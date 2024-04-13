mwifiex_clean_txrx(struct mwifiex_private *priv)
{
	struct sk_buff *skb, *tmp;

	mwifiex_11n_cleanup_reorder_tbl(priv);
	spin_lock_bh(&priv->wmm.ra_list_spinlock);

	mwifiex_wmm_cleanup_queues(priv);
	mwifiex_11n_delete_all_tx_ba_stream_tbl(priv);

	if (priv->adapter->if_ops.cleanup_mpa_buf)
		priv->adapter->if_ops.cleanup_mpa_buf(priv->adapter);

	mwifiex_wmm_delete_all_ralist(priv);
	memcpy(tos_to_tid, ac_to_tid, sizeof(tos_to_tid));

	if (priv->adapter->if_ops.clean_pcie_ring &&
	    !test_bit(MWIFIEX_SURPRISE_REMOVED, &priv->adapter->work_flags))
		priv->adapter->if_ops.clean_pcie_ring(priv->adapter);
	spin_unlock_bh(&priv->wmm.ra_list_spinlock);

	skb_queue_walk_safe(&priv->tdls_txq, skb, tmp) {
		skb_unlink(skb, &priv->tdls_txq);
		mwifiex_write_data_complete(priv->adapter, skb, 0, -1);
	}

	skb_queue_walk_safe(&priv->bypass_txq, skb, tmp) {
		skb_unlink(skb, &priv->bypass_txq);
		mwifiex_write_data_complete(priv->adapter, skb, 0, -1);
	}
	atomic_set(&priv->adapter->bypass_tx_pending, 0);

	idr_for_each(&priv->ack_status_frames, mwifiex_free_ack_frame, NULL);
	idr_destroy(&priv->ack_status_frames);
}