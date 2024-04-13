static int ath6kl_wmi_txe_notify_event_rx(struct wmi *wmi, u8 *datap, int len,
					  struct ath6kl_vif *vif)
{
	struct wmi_txe_notify_event *ev;
	u32 rate, pkts;

	if (len < sizeof(*ev))
		return -EINVAL;

	if (vif->nw_type != INFRA_NETWORK ||
	    !test_bit(ATH6KL_FW_CAPABILITY_TX_ERR_NOTIFY,
		      vif->ar->fw_capabilities))
		return -EOPNOTSUPP;

	if (vif->sme_state != SME_CONNECTED)
		return -ENOTCONN;

	ev = (struct wmi_txe_notify_event *) datap;
	rate = le32_to_cpu(ev->rate);
	pkts = le32_to_cpu(ev->pkts);

	ath6kl_dbg(ATH6KL_DBG_WMI, "TXE notify event: peer %pM rate %d%% pkts %d intvl %ds\n",
		   vif->bssid, rate, pkts, vif->txe_intvl);

	cfg80211_cqm_txe_notify(vif->ndev, vif->bssid, pkts,
				rate, vif->txe_intvl, GFP_KERNEL);

	return 0;
}