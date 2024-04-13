int ath6kl_wmi_info_req_cmd(struct wmi *wmi, u8 if_idx, u32 info_req_flags)
{
	struct sk_buff *skb;
	struct wmi_get_p2p_info *p;

	skb = ath6kl_wmi_get_new_buf(sizeof(*p));
	if (!skb)
		return -ENOMEM;

	ath6kl_dbg(ATH6KL_DBG_WMI, "info_req_cmd: flags=%x\n",
		   info_req_flags);
	p = (struct wmi_get_p2p_info *) skb->data;
	p->info_req_flags = cpu_to_le32(info_req_flags);
	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_GET_P2P_INFO_CMDID,
				   NO_SYNC_WMIFLAG);
}