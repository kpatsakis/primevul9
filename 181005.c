int ath6kl_wmi_mcast_filter_cmd(struct wmi *wmi, u8 if_idx, bool mc_all_on)
{
	struct sk_buff *skb;
	struct wmi_mcast_filter_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_mcast_filter_cmd *) skb->data;
	cmd->mcast_all_enable = mc_all_on;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_MCAST_FILTER_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}