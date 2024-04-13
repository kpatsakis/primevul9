int ath6kl_wmi_ap_profile_commit(struct wmi *wmip, u8 if_idx,
				 struct wmi_connect_cmd *p)
{
	struct sk_buff *skb;
	struct wmi_connect_cmd *cm;
	int res;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cm));
	if (!skb)
		return -ENOMEM;

	cm = (struct wmi_connect_cmd *) skb->data;
	memcpy(cm, p, sizeof(*cm));

	res = ath6kl_wmi_cmd_send(wmip, if_idx, skb, WMI_AP_CONFIG_COMMIT_CMDID,
				  NO_SYNC_WMIFLAG);
	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "%s: nw_type=%u auth_mode=%u ch=%u ctrl_flags=0x%x-> res=%d\n",
		   __func__, p->nw_type, p->auth_mode, le16_to_cpu(p->ch),
		   le32_to_cpu(p->ctrl_flags), res);
	return res;
}