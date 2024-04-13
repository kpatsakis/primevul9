static int ath6kl_wmi_bssinfo_event_rx(struct wmi *wmi, u8 *datap, int len,
				       struct ath6kl_vif *vif)
{
	struct wmi_bss_info_hdr2 *bih;
	u8 *buf;
	struct ieee80211_channel *channel;
	struct ath6kl *ar = wmi->parent_dev;
	struct cfg80211_bss *bss;

	if (len <= sizeof(struct wmi_bss_info_hdr2))
		return -EINVAL;

	bih = (struct wmi_bss_info_hdr2 *) datap;
	buf = datap + sizeof(struct wmi_bss_info_hdr2);
	len -= sizeof(struct wmi_bss_info_hdr2);

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "bss info evt - ch %u, snr %d, rssi %d, bssid \"%pM\" "
		   "frame_type=%d\n",
		   bih->ch, bih->snr, bih->snr - 95, bih->bssid,
		   bih->frame_type);

	if (bih->frame_type != BEACON_FTYPE &&
	    bih->frame_type != PROBERESP_FTYPE)
		return 0; /* Only update BSS table for now */

	if (bih->frame_type == BEACON_FTYPE &&
	    test_bit(CLEAR_BSSFILTER_ON_BEACON, &vif->flags)) {
		clear_bit(CLEAR_BSSFILTER_ON_BEACON, &vif->flags);
		ath6kl_wmi_bssfilter_cmd(ar->wmi, vif->fw_vif_idx,
					 NONE_BSS_FILTER, 0);
	}

	channel = ieee80211_get_channel(ar->wiphy, le16_to_cpu(bih->ch));
	if (channel == NULL)
		return -EINVAL;

	if (len < 8 + 2 + 2)
		return -EINVAL;

	if (bih->frame_type == BEACON_FTYPE &&
	    test_bit(CONNECTED, &vif->flags) &&
	    memcmp(bih->bssid, vif->bssid, ETH_ALEN) == 0) {
		const u8 *tim;
		tim = cfg80211_find_ie(WLAN_EID_TIM, buf + 8 + 2 + 2,
				       len - 8 - 2 - 2);
		if (tim && tim[1] >= 2) {
			vif->assoc_bss_dtim_period = tim[3];
			set_bit(DTIM_PERIOD_AVAIL, &vif->flags);
		}
	}

	bss = cfg80211_inform_bss(ar->wiphy, channel,
				  bih->frame_type == BEACON_FTYPE ?
					CFG80211_BSS_FTYPE_BEACON :
					CFG80211_BSS_FTYPE_PRESP,
				  bih->bssid, get_unaligned_le64((__le64 *)buf),
				  get_unaligned_le16(((__le16 *)buf) + 5),
				  get_unaligned_le16(((__le16 *)buf) + 4),
				  buf + 8 + 2 + 2, len - 8 - 2 - 2,
				  (bih->snr - 95) * 100, GFP_ATOMIC);
	if (bss == NULL)
		return -ENOMEM;
	cfg80211_put_bss(ar->wiphy, bss);

	/*
	 * Firmware doesn't return any event when scheduled scan has
	 * finished, so we need to use a timer to find out when there are
	 * no more results.
	 *
	 * The timer is started from the first bss info received, otherwise
	 * the timer would not ever fire if the scan interval is short
	 * enough.
	 */
	if (test_bit(SCHED_SCANNING, &vif->flags) &&
	    !timer_pending(&vif->sched_scan_timer)) {
		mod_timer(&vif->sched_scan_timer, jiffies +
			  msecs_to_jiffies(ATH6KL_SCHED_SCAN_RESULT_DELAY));
	}

	return 0;
}