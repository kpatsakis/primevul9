int ath6kl_wmi_del_wow_pattern_cmd(struct wmi *wmi, u8 if_idx,
				   u16 list_id, u16 filter_id)
{
	struct sk_buff *skb;
	struct wmi_del_wow_pattern_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_del_wow_pattern_cmd *) skb->data;
	cmd->filter_list_id = cpu_to_le16(list_id);
	cmd->filter_id = cpu_to_le16(filter_id);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_DEL_WOW_PATTERN_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}