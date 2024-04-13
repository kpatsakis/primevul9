int ath6kl_wmi_set_pvb_cmd(struct wmi *wmi, u8 if_idx, u16 aid,
			   bool flag)
{
	struct sk_buff *skb;
	struct wmi_ap_set_pvb_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_ap_set_pvb_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_ap_set_pvb_cmd *) skb->data;
	cmd->aid = cpu_to_le16(aid);
	cmd->rsvd = cpu_to_le16(0);
	cmd->flag = cpu_to_le32(flag);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_AP_SET_PVB_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}