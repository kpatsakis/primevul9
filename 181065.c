int ath6kl_wmi_powermode_cmd(struct wmi *wmi, u8 if_idx, u8 pwr_mode)
{
	struct sk_buff *skb;
	struct wmi_power_mode_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_power_mode_cmd *) skb->data;
	cmd->pwr_mode = pwr_mode;
	wmi->pwr_mode = pwr_mode;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_POWER_MODE_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}