mwifiex_wmm_setup_queue_priorities(struct mwifiex_private *priv,
				   struct ieee_types_wmm_parameter *wmm_ie)
{
	u16 cw_min, avg_back_off, tmp[4];
	u32 i, j, num_ac;
	u8 ac_idx;

	if (!wmm_ie || !priv->wmm_enabled) {
		/* WMM is not enabled, just set the defaults and return */
		mwifiex_wmm_default_queue_priorities(priv);
		return;
	}

	mwifiex_dbg(priv->adapter, INFO,
		    "info: WMM Parameter IE: version=%d,\t"
		    "qos_info Parameter Set Count=%d, Reserved=%#x\n",
		    wmm_ie->version, wmm_ie->qos_info_bitmap &
		    IEEE80211_WMM_IE_AP_QOSINFO_PARAM_SET_CNT_MASK,
		    wmm_ie->reserved);

	for (num_ac = 0; num_ac < ARRAY_SIZE(wmm_ie->ac_params); num_ac++) {
		u8 ecw = wmm_ie->ac_params[num_ac].ecw_bitmap;
		u8 aci_aifsn = wmm_ie->ac_params[num_ac].aci_aifsn_bitmap;
		cw_min = (1 << (ecw & MWIFIEX_ECW_MIN)) - 1;
		avg_back_off = (cw_min >> 1) + (aci_aifsn & MWIFIEX_AIFSN);

		ac_idx = wmm_aci_to_qidx_map[(aci_aifsn & MWIFIEX_ACI) >> 5];
		priv->wmm.queue_priority[ac_idx] = ac_idx;
		tmp[ac_idx] = avg_back_off;

		mwifiex_dbg(priv->adapter, INFO,
			    "info: WMM: CWmax=%d CWmin=%d Avg Back-off=%d\n",
			    (1 << ((ecw & MWIFIEX_ECW_MAX) >> 4)) - 1,
			    cw_min, avg_back_off);
		mwifiex_wmm_ac_debug_print(&wmm_ie->ac_params[num_ac]);
	}

	/* Bubble sort */
	for (i = 0; i < num_ac; i++) {
		for (j = 1; j < num_ac - i; j++) {
			if (tmp[j - 1] > tmp[j]) {
				swap(tmp[j - 1], tmp[j]);
				swap(priv->wmm.queue_priority[j - 1],
				     priv->wmm.queue_priority[j]);
			} else if (tmp[j - 1] == tmp[j]) {
				if (priv->wmm.queue_priority[j - 1]
				    < priv->wmm.queue_priority[j])
					swap(priv->wmm.queue_priority[j - 1],
					     priv->wmm.queue_priority[j]);
			}
		}
	}

	mwifiex_wmm_queue_priorities_tid(priv);
}