static int ath6kl_wmi_p2p_info_event_rx(u8 *datap, int len)
{
	struct wmi_p2p_info_event *ev;
	u32 flags;
	u16 dlen;

	if (len < sizeof(*ev))
		return -EINVAL;

	ev = (struct wmi_p2p_info_event *) datap;
	flags = le32_to_cpu(ev->info_req_flags);
	dlen = le16_to_cpu(ev->len);
	ath6kl_dbg(ATH6KL_DBG_WMI, "p2p_info: flags=%x len=%d\n", flags, dlen);

	if (flags & P2P_FLAG_CAPABILITIES_REQ) {
		struct wmi_p2p_capabilities *cap;
		if (dlen < sizeof(*cap))
			return -EINVAL;
		cap = (struct wmi_p2p_capabilities *) ev->data;
		ath6kl_dbg(ATH6KL_DBG_WMI, "p2p_info: GO Power Save = %d\n",
			   cap->go_power_save);
	}

	if (flags & P2P_FLAG_MACADDR_REQ) {
		struct wmi_p2p_macaddr *mac;
		if (dlen < sizeof(*mac))
			return -EINVAL;
		mac = (struct wmi_p2p_macaddr *) ev->data;
		ath6kl_dbg(ATH6KL_DBG_WMI, "p2p_info: MAC Address = %pM\n",
			   mac->mac_addr);
	}

	if (flags & P2P_FLAG_HMODEL_REQ) {
		struct wmi_p2p_hmodel *mod;
		if (dlen < sizeof(*mod))
			return -EINVAL;
		mod = (struct wmi_p2p_hmodel *) ev->data;
		ath6kl_dbg(ATH6KL_DBG_WMI, "p2p_info: P2P Model = %d (%s)\n",
			   mod->p2p_model,
			   mod->p2p_model ? "host" : "firmware");
	}
	return 0;
}