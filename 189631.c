__acquires(&sta->tid_rx_lock) __releases(&sta->tid_rx_lock)
{
	struct wmi_delba_event *evt = d;
	u8 cid, tid;
	u16 reason = __le16_to_cpu(evt->reason);
	struct wil_sta_info *sta;
	struct wil_tid_ampdu_rx *r;

	might_sleep();
	parse_cidxtid(evt->cidxtid, &cid, &tid);
	wil_dbg_wmi(wil, "DELBA CID %d TID %d from %s reason %d\n",
		    cid, tid,
		    evt->from_initiator ? "originator" : "recipient",
		    reason);
	if (!evt->from_initiator) {
		int i;
		/* find Tx vring it belongs to */
		for (i = 0; i < ARRAY_SIZE(wil->vring2cid_tid); i++) {
			if ((wil->vring2cid_tid[i][0] == cid) &&
			    (wil->vring2cid_tid[i][1] == tid)) {
				struct vring_tx_data *txdata =
					&wil->vring_tx_data[i];

				wil_dbg_wmi(wil, "DELBA Tx vring %d\n", i);
				txdata->agg_timeout = 0;
				txdata->agg_wsize = 0;
				txdata->addba_in_progress = false;

				break; /* max. 1 matching ring */
			}
		}
		if (i >= ARRAY_SIZE(wil->vring2cid_tid))
			wil_err(wil, "DELBA: unable to find Tx vring\n");
		return;
	}

	sta = &wil->sta[cid];

	spin_lock_bh(&sta->tid_rx_lock);

	r = sta->tid_rx[tid];
	sta->tid_rx[tid] = NULL;
	wil_tid_ampdu_rx_free(wil, r);

	spin_unlock_bh(&sta->tid_rx_lock);
}