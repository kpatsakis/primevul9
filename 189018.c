static void i40e_fdir_flush_and_replay(struct i40e_pf *pf)
{
	unsigned long min_flush_time;
	int flush_wait_retry = 50;
	bool disable_atr = false;
	int fd_room;
	int reg;

	if (!time_after(jiffies, pf->fd_flush_timestamp +
				 (I40E_MIN_FD_FLUSH_INTERVAL * HZ)))
		return;

	/* If the flush is happening too quick and we have mostly SB rules we
	 * should not re-enable ATR for some time.
	 */
	min_flush_time = pf->fd_flush_timestamp +
			 (I40E_MIN_FD_FLUSH_SB_ATR_UNSTABLE * HZ);
	fd_room = pf->fdir_pf_filter_count - pf->fdir_pf_active_filters;

	if (!(time_after(jiffies, min_flush_time)) &&
	    (fd_room < I40E_FDIR_BUFFER_HEAD_ROOM_FOR_ATR)) {
		if (I40E_DEBUG_FD & pf->hw.debug_mask)
			dev_info(&pf->pdev->dev, "ATR disabled, not enough FD filter space.\n");
		disable_atr = true;
	}

	pf->fd_flush_timestamp = jiffies;
	set_bit(__I40E_FD_ATR_AUTO_DISABLED, pf->state);
	/* flush all filters */
	wr32(&pf->hw, I40E_PFQF_CTL_1,
	     I40E_PFQF_CTL_1_CLEARFDTABLE_MASK);
	i40e_flush(&pf->hw);
	pf->fd_flush_cnt++;
	pf->fd_add_err = 0;
	do {
		/* Check FD flush status every 5-6msec */
		usleep_range(5000, 6000);
		reg = rd32(&pf->hw, I40E_PFQF_CTL_1);
		if (!(reg & I40E_PFQF_CTL_1_CLEARFDTABLE_MASK))
			break;
	} while (flush_wait_retry--);
	if (reg & I40E_PFQF_CTL_1_CLEARFDTABLE_MASK) {
		dev_warn(&pf->pdev->dev, "FD table did not flush, needs more time\n");
	} else {
		/* replay sideband filters */
		i40e_fdir_filter_restore(pf->vsi[pf->lan_vsi]);
		if (!disable_atr && !pf->fd_tcp4_filter_cnt)
			clear_bit(__I40E_FD_ATR_AUTO_DISABLED, pf->state);
		clear_bit(__I40E_FD_FLUSH_REQUESTED, pf->state);
		if (I40E_DEBUG_FD & pf->hw.debug_mask)
			dev_info(&pf->pdev->dev, "FD Filter table flushed and FD-SB replayed.\n");
	}
}