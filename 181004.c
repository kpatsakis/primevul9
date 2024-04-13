int ath6kl_wmi_set_ip_cmd(struct wmi *wmi, u8 if_idx,
			  __be32 ips0, __be32 ips1)
{
	struct sk_buff *skb;
	struct wmi_set_ip_cmd *cmd;
	int ret;

	/* Multicast address are not valid */
	if (ipv4_is_multicast(ips0) ||
	    ipv4_is_multicast(ips1))
		return -EINVAL;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_set_ip_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_ip_cmd *) skb->data;
	cmd->ips[0] = ips0;
	cmd->ips[1] = ips1;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_IP_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}