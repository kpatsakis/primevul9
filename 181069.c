int ath6kl_wmi_ap_set_beacon_intvl_cmd(struct wmi *wmi, u8 if_idx,
				       u32 beacon_intvl)
{
	struct sk_buff *skb;
	struct set_beacon_int_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct set_beacon_int_cmd *) skb->data;

	cmd->beacon_intvl = cpu_to_le32(beacon_intvl);
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				   WMI_SET_BEACON_INT_CMDID, NO_SYNC_WMIFLAG);
}