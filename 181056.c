static int ath6kl_wmi_cmd_send_xtnd(struct wmi *wmi, struct sk_buff *skb,
				    enum wmix_command_id cmd_id,
				    enum wmi_sync_flag sync_flag)
{
	struct wmix_cmd_hdr *cmd_hdr;
	int ret;

	skb_push(skb, sizeof(struct wmix_cmd_hdr));

	cmd_hdr = (struct wmix_cmd_hdr *) skb->data;
	cmd_hdr->cmd_id = cpu_to_le32(cmd_id);

	ret = ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_EXTENSION_CMDID, sync_flag);

	return ret;
}