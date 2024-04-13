int ath6kl_wmi_bssfilter_cmd(struct wmi *wmi, u8 if_idx, u8 filter, u32 ie_mask)
{
	struct sk_buff *skb;
	struct wmi_bss_filter_cmd *cmd;
	int ret;

	if (filter >= LAST_BSS_FILTER)
		return -EINVAL;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_bss_filter_cmd *) skb->data;
	cmd->bss_filter = filter;
	cmd->ie_mask = cpu_to_le32(ie_mask);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_BSS_FILTER_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}