static int ath6kl_wmi_control_rx_xtnd(struct wmi *wmi, struct sk_buff *skb)
{
	struct wmix_cmd_hdr *cmd;
	u32 len;
	u16 id;
	u8 *datap;
	int ret = 0;

	if (skb->len < sizeof(struct wmix_cmd_hdr)) {
		ath6kl_err("bad packet 1\n");
		return -EINVAL;
	}

	cmd = (struct wmix_cmd_hdr *) skb->data;
	id = le32_to_cpu(cmd->cmd_id);

	skb_pull(skb, sizeof(struct wmix_cmd_hdr));

	datap = skb->data;
	len = skb->len;

	switch (id) {
	case WMIX_HB_CHALLENGE_RESP_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "wmi event hb challenge resp\n");
		ath6kl_wmi_hb_challenge_resp_event(wmi, datap, len);
		break;
	case WMIX_DBGLOG_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "wmi event dbglog len %d\n", len);
		ath6kl_debug_fwlog_event(wmi->parent_dev, datap, len);
		break;
	default:
		ath6kl_warn("unknown cmd id 0x%x\n", id);
		ret = -EINVAL;
		break;
	}

	return ret;
}