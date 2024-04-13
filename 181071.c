int ath6kl_wmi_set_tx_pwr_cmd(struct wmi *wmi, u8 if_idx, u8 dbM)
{
	struct sk_buff *skb;
	struct wmi_set_tx_pwr_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_set_tx_pwr_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_tx_pwr_cmd *) skb->data;
	cmd->dbM = dbM;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_TX_PWR_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}