int ath6kl_wmi_send_probe_response_cmd(struct wmi *wmi, u8 if_idx, u32 freq,
				       const u8 *dst, const u8 *data,
				       u16 data_len)
{
	struct sk_buff *skb;
	struct wmi_p2p_probe_response_cmd *p;
	size_t cmd_len = sizeof(*p) + data_len;

	if (data_len == 0)
		cmd_len++; /* work around target minimum length requirement */

	skb = ath6kl_wmi_get_new_buf(cmd_len);
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "send_probe_response_cmd: freq=%u dst=%pM len=%u\n",
		   freq, dst, data_len);
	p = (struct wmi_p2p_probe_response_cmd *) skb->data;
	p->freq = cpu_to_le32(freq);
	memcpy(p->destination_addr, dst, ETH_ALEN);
	p->len = cpu_to_le16(data_len);
	memcpy(p->data, data, data_len);
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				   WMI_SEND_PROBE_RESPONSE_CMDID,
				   NO_SYNC_WMIFLAG);
}