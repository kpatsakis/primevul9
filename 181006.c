static int __ath6kl_wmi_send_mgmt_cmd(struct wmi *wmi, u8 if_idx, u32 id,
				      u32 freq, u32 wait, const u8 *data,
				      u16 data_len, u32 no_cck)
{
	struct sk_buff *skb;
	struct wmi_send_mgmt_cmd *p;
	u8 *buf;

	if (wait)
		return -EINVAL; /* Offload for wait not supported */

	buf = kmalloc(data_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	skb = ath6kl_wmi_get_new_buf(sizeof(*p) + data_len);
	if (!skb) {
		kfree(buf);
		return -ENOMEM;
	}

	kfree(wmi->last_mgmt_tx_frame);
	memcpy(buf, data, data_len);
	wmi->last_mgmt_tx_frame = buf;
	wmi->last_mgmt_tx_frame_len = data_len;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "send_action_cmd: id=%u freq=%u wait=%u len=%u\n",
		   id, freq, wait, data_len);
	p = (struct wmi_send_mgmt_cmd *) skb->data;
	p->id = cpu_to_le32(id);
	p->freq = cpu_to_le32(freq);
	p->wait = cpu_to_le32(wait);
	p->no_cck = cpu_to_le32(no_cck);
	p->len = cpu_to_le16(data_len);
	memcpy(p->data, data, data_len);
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SEND_MGMT_CMDID,
				   NO_SYNC_WMIFLAG);
}