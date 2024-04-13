int ath6kl_wmi_set_host_sleep_mode_cmd(struct wmi *wmi, u8 if_idx,
				       enum ath6kl_host_mode host_mode)
{
	struct sk_buff *skb;
	struct wmi_set_host_sleep_mode_cmd *cmd;
	int ret;

	if ((host_mode != ATH6KL_HOST_MODE_ASLEEP) &&
	    (host_mode != ATH6KL_HOST_MODE_AWAKE)) {
		ath6kl_err("invalid host sleep mode: %d\n", host_mode);
		return -EINVAL;
	}

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_host_sleep_mode_cmd *) skb->data;

	if (host_mode == ATH6KL_HOST_MODE_ASLEEP) {
		ath6kl_wmi_relinquish_implicit_pstream_credits(wmi);
		cmd->asleep = cpu_to_le32(1);
	} else {
		cmd->awake = cpu_to_le32(1);
	}

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				  WMI_SET_HOST_SLEEP_MODE_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}