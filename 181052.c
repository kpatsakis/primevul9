static int ath6kl_wmi_cancel_remain_on_chnl_event_rx(struct wmi *wmi,
						     u8 *datap, int len,
						     struct ath6kl_vif *vif)
{
	struct wmi_cancel_remain_on_chnl_event *ev;
	u32 freq;
	u32 dur;
	struct ieee80211_channel *chan;
	struct ath6kl *ar = wmi->parent_dev;
	u32 id;

	if (len < sizeof(*ev))
		return -EINVAL;

	ev = (struct wmi_cancel_remain_on_chnl_event *) datap;
	freq = le32_to_cpu(ev->freq);
	dur = le32_to_cpu(ev->duration);
	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "cancel_remain_on_chnl: freq=%u dur=%u status=%u\n",
		   freq, dur, ev->status);
	chan = ieee80211_get_channel(ar->wiphy, freq);
	if (!chan) {
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "cancel_remain_on_chnl: Unknown channel (freq=%u)\n",
			   freq);
		return -EINVAL;
	}
	if (vif->last_cancel_roc_id &&
	    vif->last_cancel_roc_id + 1 == vif->last_roc_id)
		id = vif->last_cancel_roc_id; /* event for cancel command */
	else
		id = vif->last_roc_id; /* timeout on uncanceled r-o-c */
	vif->last_cancel_roc_id = 0;
	cfg80211_remain_on_channel_expired(&vif->wdev, id, chan, GFP_ATOMIC);

	return 0;
}