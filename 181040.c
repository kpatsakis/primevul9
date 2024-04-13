int ath6kl_wmi_set_keepalive_cmd(struct wmi *wmi, u8 if_idx,
				 u8 keep_alive_intvl)
{
	struct sk_buff *skb;
	struct wmi_set_keepalive_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_keepalive_cmd *) skb->data;
	cmd->keep_alive_intvl = keep_alive_intvl;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_KEEPALIVE_CMDID,
				  NO_SYNC_WMIFLAG);

	if (ret == 0)
		ath6kl_debug_set_keepalive(wmi->parent_dev, keep_alive_intvl);

	return ret;
}