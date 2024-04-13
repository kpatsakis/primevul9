int ath6kl_wmi_set_appie_cmd(struct wmi *wmi, u8 if_idx, u8 mgmt_frm_type,
			     const u8 *ie, u8 ie_len)
{
	struct sk_buff *skb;
	struct wmi_set_appie_cmd *p;

	skb = ath6kl_wmi_get_new_buf(sizeof(*p) + ie_len);
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "set_appie_cmd: mgmt_frm_type=%u ie_len=%u\n",
		   mgmt_frm_type, ie_len);
	p = (struct wmi_set_appie_cmd *) skb->data;
	p->mgmt_frm_type = mgmt_frm_type;
	p->ie_len = ie_len;

	if (ie != NULL && ie_len > 0)
		memcpy(p->ie_info, ie, ie_len);

	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_APPIE_CMDID,
				   NO_SYNC_WMIFLAG);
}