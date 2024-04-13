mwifiex_wmm_init(struct mwifiex_adapter *adapter)
{
	int i, j;
	struct mwifiex_private *priv;

	for (j = 0; j < adapter->priv_num; ++j) {
		priv = adapter->priv[j];
		if (!priv)
			continue;

		for (i = 0; i < MAX_NUM_TID; ++i) {
			if (!disable_tx_amsdu &&
			    adapter->tx_buf_size > MWIFIEX_TX_DATA_BUF_SIZE_2K)
				priv->aggr_prio_tbl[i].amsdu =
							priv->tos_to_tid_inv[i];
			else
				priv->aggr_prio_tbl[i].amsdu =
							BA_STREAM_NOT_ALLOWED;
			priv->aggr_prio_tbl[i].ampdu_ap =
							priv->tos_to_tid_inv[i];
			priv->aggr_prio_tbl[i].ampdu_user =
							priv->tos_to_tid_inv[i];
		}

		priv->aggr_prio_tbl[6].amsdu
					= priv->aggr_prio_tbl[6].ampdu_ap
					= priv->aggr_prio_tbl[6].ampdu_user
					= BA_STREAM_NOT_ALLOWED;

		priv->aggr_prio_tbl[7].amsdu = priv->aggr_prio_tbl[7].ampdu_ap
					= priv->aggr_prio_tbl[7].ampdu_user
					= BA_STREAM_NOT_ALLOWED;

		mwifiex_set_ba_params(priv);
		mwifiex_reset_11n_rx_seq_num(priv);

		priv->wmm.drv_pkt_delay_max = MWIFIEX_WMM_DRV_DELAY_MAX;
		atomic_set(&priv->wmm.tx_pkts_queued, 0);
		atomic_set(&priv->wmm.highest_queued_prio, HIGH_PRIO_TID);
	}
}