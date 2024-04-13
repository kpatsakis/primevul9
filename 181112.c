int ath6kl_wmi_add_wow_pattern_cmd(struct wmi *wmi, u8 if_idx,
				   u8 list_id, u8 filter_size,
				   u8 filter_offset, const u8 *filter,
				   const u8 *mask)
{
	struct sk_buff *skb;
	struct wmi_add_wow_pattern_cmd *cmd;
	u16 size;
	u8 *filter_mask;
	int ret;

	/*
	 * Allocate additional memory in the buffer to hold
	 * filter and mask value, which is twice of filter_size.
	 */
	size = sizeof(*cmd) + (2 * filter_size);

	skb = ath6kl_wmi_get_new_buf(size);
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_add_wow_pattern_cmd *) skb->data;
	cmd->filter_list_id = list_id;
	cmd->filter_size = filter_size;
	cmd->filter_offset = filter_offset;

	memcpy(cmd->filter, filter, filter_size);

	filter_mask = (u8 *) (cmd->filter + filter_size);
	memcpy(filter_mask, mask, filter_size);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_ADD_WOW_PATTERN_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}