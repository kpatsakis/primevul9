int ath6kl_wmi_get_challenge_resp_cmd(struct wmi *wmi, u32 cookie, u32 source)
{
	struct sk_buff *skb;
	struct wmix_hb_challenge_resp_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmix_hb_challenge_resp_cmd *) skb->data;
	cmd->cookie = cpu_to_le32(cookie);
	cmd->source = cpu_to_le32(source);

	ret = ath6kl_wmi_cmd_send_xtnd(wmi, skb, WMIX_HB_CHALLENGE_RESP_CMDID,
				       NO_SYNC_WMIFLAG);
	return ret;
}