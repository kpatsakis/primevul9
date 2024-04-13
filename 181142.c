int ath6kl_wmi_enable_sched_scan_cmd(struct wmi *wmi, u8 if_idx, bool enable)
{
	struct sk_buff *skb;
	struct wmi_enable_sched_scan_cmd *sc;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*sc));
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI, "%s scheduled scan on vif %d\n",
		   enable ? "enabling" : "disabling", if_idx);
	sc = (struct wmi_enable_sched_scan_cmd *) skb->data;
	sc->enable = enable ? 1 : 0;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				  WMI_ENABLE_SCHED_SCAN_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}