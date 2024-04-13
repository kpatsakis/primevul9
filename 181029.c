int ath6kl_wmi_set_wow_mode_cmd(struct wmi *wmi, u8 if_idx,
				enum ath6kl_wow_mode wow_mode,
				u32 filter, u16 host_req_delay)
{
	struct sk_buff *skb;
	struct wmi_set_wow_mode_cmd *cmd;
	int ret;

	if ((wow_mode != ATH6KL_WOW_MODE_ENABLE) &&
	    wow_mode != ATH6KL_WOW_MODE_DISABLE) {
		ath6kl_err("invalid wow mode: %d\n", wow_mode);
		return -EINVAL;
	}

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_wow_mode_cmd *) skb->data;
	cmd->enable_wow = cpu_to_le32(wow_mode);
	cmd->filter = cpu_to_le32(filter);
	cmd->host_req_delay = cpu_to_le16(host_req_delay);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_WOW_MODE_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}