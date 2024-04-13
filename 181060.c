int ath6kl_wmi_probe_report_req_cmd(struct wmi *wmi, u8 if_idx, bool enable)
{
	struct sk_buff *skb;
	struct wmi_probe_req_report_cmd *p;

	skb = ath6kl_wmi_get_new_buf(sizeof(*p));
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI, "probe_report_req_cmd: enable=%u\n",
		   enable);
	p = (struct wmi_probe_req_report_cmd *) skb->data;
	p->enable = enable ? 1 : 0;
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_PROBE_REQ_REPORT_CMDID,
				   NO_SYNC_WMIFLAG);
}