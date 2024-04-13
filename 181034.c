int ath6kl_wmi_add_krk_cmd(struct wmi *wmi, u8 if_idx, const u8 *krk)
{
	struct sk_buff *skb;
	struct wmi_add_krk_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_add_krk_cmd *) skb->data;
	memcpy(cmd->krk, krk, WMI_KRK_LEN);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_ADD_KRK_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}