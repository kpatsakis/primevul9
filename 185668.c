mwifiex_dequeue_tx_packet(struct mwifiex_adapter *adapter)
{
	struct mwifiex_ra_list_tbl *ptr;
	struct mwifiex_private *priv = NULL;
	int ptr_index = 0;
	u8 ra[ETH_ALEN];
	int tid_del = 0, tid = 0;

	ptr = mwifiex_wmm_get_highest_priolist_ptr(adapter, &priv, &ptr_index);
	if (!ptr)
		return -1;

	tid = mwifiex_get_tid(ptr);

	mwifiex_dbg(adapter, DATA, "data: tid=%d\n", tid);

	spin_lock_bh(&priv->wmm.ra_list_spinlock);
	if (!mwifiex_is_ralist_valid(priv, ptr, ptr_index)) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		return -1;
	}

	if (mwifiex_is_ptr_processed(priv, ptr)) {
		mwifiex_send_processed_packet(priv, ptr, ptr_index);
		/* ra_list_spinlock has been freed in
		   mwifiex_send_processed_packet() */
		return 0;
	}

	if (!ptr->is_11n_enabled ||
		ptr->ba_status ||
		priv->wps.session_enable) {
		if (ptr->is_11n_enabled &&
			ptr->ba_status &&
			ptr->amsdu_in_ampdu &&
			mwifiex_is_amsdu_allowed(priv, tid) &&
			mwifiex_is_11n_aggragation_possible(priv, ptr,
							adapter->tx_buf_size))
			mwifiex_11n_aggregate_pkt(priv, ptr, ptr_index);
			/* ra_list_spinlock has been freed in
			 * mwifiex_11n_aggregate_pkt()
			 */
		else
			mwifiex_send_single_packet(priv, ptr, ptr_index);
			/* ra_list_spinlock has been freed in
			 * mwifiex_send_single_packet()
			 */
	} else {
		if (mwifiex_is_ampdu_allowed(priv, ptr, tid) &&
		    ptr->ba_pkt_count > ptr->ba_packet_thr) {
			if (mwifiex_space_avail_for_new_ba_stream(adapter)) {
				mwifiex_create_ba_tbl(priv, ptr->ra, tid,
						      BA_SETUP_INPROGRESS);
				mwifiex_send_addba(priv, tid, ptr->ra);
			} else if (mwifiex_find_stream_to_delete
				   (priv, tid, &tid_del, ra)) {
				mwifiex_create_ba_tbl(priv, ptr->ra, tid,
						      BA_SETUP_INPROGRESS);
				mwifiex_send_delba(priv, tid_del, ra, 1);
			}
		}
		if (mwifiex_is_amsdu_allowed(priv, tid) &&
		    mwifiex_is_11n_aggragation_possible(priv, ptr,
							adapter->tx_buf_size))
			mwifiex_11n_aggregate_pkt(priv, ptr, ptr_index);
			/* ra_list_spinlock has been freed in
			   mwifiex_11n_aggregate_pkt() */
		else
			mwifiex_send_single_packet(priv, ptr, ptr_index);
			/* ra_list_spinlock has been freed in
			   mwifiex_send_single_packet() */
	}
	return 0;
}