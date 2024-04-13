int ath6kl_wmi_set_wmm_txop(struct wmi *wmi, u8 if_idx, enum wmi_txop_cfg cfg)
{
	struct sk_buff *skb;
	struct wmi_set_wmm_txop_cmd *cmd;
	int ret;

	if (!((cfg == WMI_TXOP_DISABLED) || (cfg == WMI_TXOP_ENABLED)))
		return -EINVAL;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_set_wmm_txop_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_wmm_txop_cmd *) skb->data;
	cmd->txop_enable = cfg;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_WMM_TXOP_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}