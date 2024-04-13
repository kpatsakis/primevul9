int ath6kl_wmi_create_pstream_cmd(struct wmi *wmi, u8 if_idx,
				  struct wmi_create_pstream_cmd *params)
{
	struct sk_buff *skb;
	struct wmi_create_pstream_cmd *cmd;
	u8 fatpipe_exist_for_ac = 0;
	s32 min_phy = 0;
	s32 nominal_phy = 0;
	int ret;

	if (!((params->user_pri <= 0x7) &&
	      (up_to_ac[params->user_pri & 0x7] == params->traffic_class) &&
	      (params->traffic_direc == UPLINK_TRAFFIC ||
	       params->traffic_direc == DNLINK_TRAFFIC ||
	       params->traffic_direc == BIDIR_TRAFFIC) &&
	      (params->traffic_type == TRAFFIC_TYPE_APERIODIC ||
	       params->traffic_type == TRAFFIC_TYPE_PERIODIC) &&
	      (params->voice_psc_cap == DISABLE_FOR_THIS_AC ||
	       params->voice_psc_cap == ENABLE_FOR_THIS_AC ||
	       params->voice_psc_cap == ENABLE_FOR_ALL_AC) &&
	      (params->tsid == WMI_IMPLICIT_PSTREAM ||
	       params->tsid <= WMI_MAX_THINSTREAM))) {
		return -EINVAL;
	}

	/*
	 * Check nominal PHY rate is >= minimalPHY,
	 * so that DUT can allow TSRS IE
	 */

	/* Get the physical rate (units of bps) */
	min_phy = ((le32_to_cpu(params->min_phy_rate) / 1000) / 1000);

	/* Check minimal phy < nominal phy rate */
	if (params->nominal_phy >= min_phy) {
		/* unit of 500 kbps */
		nominal_phy = (params->nominal_phy * 1000) / 500;
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "TSRS IE enabled::MinPhy %x->NominalPhy ===> %x\n",
			   min_phy, nominal_phy);

		params->nominal_phy = nominal_phy;
	} else {
		params->nominal_phy = 0;
	}

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "sending create_pstream_cmd: ac=%d  tsid:%d\n",
		   params->traffic_class, params->tsid);

	cmd = (struct wmi_create_pstream_cmd *) skb->data;
	memcpy(cmd, params, sizeof(*cmd));

	/* This is an implicitly created Fat pipe */
	if ((u32) params->tsid == (u32) WMI_IMPLICIT_PSTREAM) {
		spin_lock_bh(&wmi->lock);
		fatpipe_exist_for_ac = (wmi->fat_pipe_exist &
					(1 << params->traffic_class));
		wmi->fat_pipe_exist |= (1 << params->traffic_class);
		spin_unlock_bh(&wmi->lock);
	} else {
		/* explicitly created thin stream within a fat pipe */
		spin_lock_bh(&wmi->lock);
		fatpipe_exist_for_ac = (wmi->fat_pipe_exist &
					(1 << params->traffic_class));
		wmi->stream_exist_for_ac[params->traffic_class] |=
		    (1 << params->tsid);
		/*
		 * If a thinstream becomes active, the fat pipe automatically
		 * becomes active
		 */
		wmi->fat_pipe_exist |= (1 << params->traffic_class);
		spin_unlock_bh(&wmi->lock);
	}

	/*
	 * Indicate activty change to driver layer only if this is the
	 * first TSID to get created in this AC explicitly or an implicit
	 * fat pipe is getting created.
	 */
	if (!fatpipe_exist_for_ac)
		ath6kl_indicate_tx_activity(wmi->parent_dev,
					    params->traffic_class, true);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_CREATE_PSTREAM_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}