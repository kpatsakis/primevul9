int ath6kl_wmi_listeninterval_cmd(struct wmi *wmi, u8 if_idx,
				  u16 listen_interval,
				  u16 listen_beacons)
{
	struct sk_buff *skb;
	struct wmi_listen_int_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_listen_int_cmd *) skb->data;
	cmd->listen_intvl = cpu_to_le16(listen_interval);
	cmd->num_beacons = cpu_to_le16(listen_beacons);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_LISTEN_INT_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}