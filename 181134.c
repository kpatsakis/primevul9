int ath6kl_wmi_setpmkid_cmd(struct wmi *wmi, u8 if_idx, const u8 *bssid,
			    const u8 *pmkid, bool set)
{
	struct sk_buff *skb;
	struct wmi_setpmkid_cmd *cmd;
	int ret;

	if (bssid == NULL)
		return -EINVAL;

	if (set && pmkid == NULL)
		return -EINVAL;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_setpmkid_cmd *) skb->data;
	memcpy(cmd->bssid, bssid, ETH_ALEN);
	if (set) {
		memcpy(cmd->pmkid, pmkid, sizeof(cmd->pmkid));
		cmd->enable = PMKID_ENABLE;
	} else {
		memset(cmd->pmkid, 0, sizeof(cmd->pmkid));
		cmd->enable = PMKID_DISABLE;
	}

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_PMKID_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}