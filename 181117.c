static int ath6kl_wmi_tx_status_event_rx(struct wmi *wmi, u8 *datap, int len,
					 struct ath6kl_vif *vif)
{
	struct wmi_tx_status_event *ev;
	u32 id;

	if (len < sizeof(*ev))
		return -EINVAL;

	ev = (struct wmi_tx_status_event *) datap;
	id = le32_to_cpu(ev->id);
	ath6kl_dbg(ATH6KL_DBG_WMI, "tx_status: id=%x ack_status=%u\n",
		   id, ev->ack_status);
	if (wmi->last_mgmt_tx_frame) {
		cfg80211_mgmt_tx_status(&vif->wdev, id,
					wmi->last_mgmt_tx_frame,
					wmi->last_mgmt_tx_frame_len,
					!!ev->ack_status, GFP_ATOMIC);
		kfree(wmi->last_mgmt_tx_frame);
		wmi->last_mgmt_tx_frame = NULL;
		wmi->last_mgmt_tx_frame_len = 0;
	}

	return 0;
}