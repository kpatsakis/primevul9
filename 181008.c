int ath6kl_wmi_ap_set_dtim_cmd(struct wmi *wmi, u8 if_idx, u32 dtim_period)
{
	struct sk_buff *skb;
	struct set_dtim_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct set_dtim_cmd *) skb->data;

	cmd->dtim_period = cpu_to_le32(dtim_period);
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				   WMI_AP_SET_DTIM_CMDID, NO_SYNC_WMIFLAG);
}