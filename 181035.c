int ath6kl_wmi_sta_bmiss_enhance_cmd(struct wmi *wmi, u8 if_idx, bool enhance)
{
	struct sk_buff *skb;
	struct wmi_sta_bmiss_enhance_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_sta_bmiss_enhance_cmd *) skb->data;
	cmd->enable = enhance ? 1 : 0;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				  WMI_STA_BMISS_ENHANCE_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}