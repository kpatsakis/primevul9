int ath6kl_wmi_set_regdomain_cmd(struct wmi *wmi, const char *alpha2)
{
	struct sk_buff *skb;
	struct wmi_set_regdomain_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_regdomain_cmd *) skb->data;
	memcpy(cmd->iso_name, alpha2, 2);

	return ath6kl_wmi_cmd_send(wmi, 0, skb,
				   WMI_SET_REGDOMAIN_CMDID,
				   NO_SYNC_WMIFLAG);
}