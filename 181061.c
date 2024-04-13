int ath6kl_wmi_set_ie_cmd(struct wmi *wmi, u8 if_idx, u8 ie_id, u8 ie_field,
			  const u8 *ie_info, u8 ie_len)
{
	struct sk_buff *skb;
	struct wmi_set_ie_cmd *p;

	skb = ath6kl_wmi_get_new_buf(sizeof(*p) + ie_len);
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI, "set_ie_cmd: ie_id=%u ie_ie_field=%u ie_len=%u\n",
		   ie_id, ie_field, ie_len);
	p = (struct wmi_set_ie_cmd *) skb->data;
	p->ie_id = ie_id;
	p->ie_field = ie_field;
	p->ie_len = ie_len;
	if (ie_info && ie_len > 0)
		memcpy(p->ie_info, ie_info, ie_len);

	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_IE_CMDID,
				   NO_SYNC_WMIFLAG);
}