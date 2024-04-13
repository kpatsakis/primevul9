int ath6kl_wmi_probedssid_cmd(struct wmi *wmi, u8 if_idx, u8 index, u8 flag,
			      u8 ssid_len, u8 *ssid)
{
	struct sk_buff *skb;
	struct wmi_probed_ssid_cmd *cmd;
	int ret;

	if (index >= MAX_PROBED_SSIDS)
		return -EINVAL;

	if (ssid_len > sizeof(cmd->ssid))
		return -EINVAL;

	if ((flag & (DISABLE_SSID_FLAG | ANY_SSID_FLAG)) && (ssid_len > 0))
		return -EINVAL;

	if ((flag & SPECIFIC_SSID_FLAG) && !ssid_len)
		return -EINVAL;

	if (flag & SPECIFIC_SSID_FLAG)
		wmi->is_probe_ssid = true;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_probed_ssid_cmd *) skb->data;
	cmd->entry_index = index;
	cmd->flag = flag;
	cmd->ssid_len = ssid_len;
	memcpy(cmd->ssid, ssid, ssid_len);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_PROBED_SSID_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}