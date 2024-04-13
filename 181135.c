static void ath6kl_wmi_relinquish_implicit_pstream_credits(struct wmi *wmi)
{
	u16 active_tsids;
	u8 stream_exist;
	int i;

	/*
	 * Relinquish credits from all implicitly created pstreams
	 * since when we go to sleep. If user created explicit
	 * thinstreams exists with in a fatpipe leave them intact
	 * for the user to delete.
	 */
	spin_lock_bh(&wmi->lock);
	stream_exist = wmi->fat_pipe_exist;
	spin_unlock_bh(&wmi->lock);

	for (i = 0; i < WMM_NUM_AC; i++) {
		if (stream_exist & (1 << i)) {
			/*
			 * FIXME: Is this lock & unlock inside
			 * for loop correct? may need rework.
			 */
			spin_lock_bh(&wmi->lock);
			active_tsids = wmi->stream_exist_for_ac[i];
			spin_unlock_bh(&wmi->lock);

			/*
			 * If there are no user created thin streams
			 * delete the fatpipe
			 */
			if (!active_tsids) {
				stream_exist &= ~(1 << i);
				/*
				 * Indicate inactivity to driver layer for
				 * this fatpipe (pstream)
				 */
				ath6kl_indicate_tx_activity(wmi->parent_dev,
							    i, false);
			}
		}
	}

	/* FIXME: Can we do this assignment without locking ? */
	spin_lock_bh(&wmi->lock);
	wmi->fat_pipe_exist = stream_exist;
	spin_unlock_bh(&wmi->lock);
}