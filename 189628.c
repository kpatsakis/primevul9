int wmi_rx_chain_add(struct wil6210_priv *wil, struct vring *vring)
{
	struct wireless_dev *wdev = wil->wdev;
	struct net_device *ndev = wil_to_ndev(wil);
	struct wmi_cfg_rx_chain_cmd cmd = {
		.action = WMI_RX_CHAIN_ADD,
		.rx_sw_ring = {
			.max_mpdu_size = cpu_to_le16(
				wil_mtu2macbuf(wil->rx_buf_len)),
			.ring_mem_base = cpu_to_le64(vring->pa),
			.ring_size = cpu_to_le16(vring->size),
		},
		.mid = 0, /* TODO - what is it? */
		.decap_trans_type = WMI_DECAP_TYPE_802_3,
		.reorder_type = WMI_RX_SW_REORDER,
		.host_thrsh = cpu_to_le16(rx_ring_overflow_thrsh),
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_cfg_rx_chain_done_event evt;
	} __packed evt;
	int rc;

	if (wdev->iftype == NL80211_IFTYPE_MONITOR) {
		struct ieee80211_channel *ch = wdev->preset_chandef.chan;

		cmd.sniffer_cfg.mode = cpu_to_le32(WMI_SNIFFER_ON);
		if (ch)
			cmd.sniffer_cfg.channel = ch->hw_value - 1;
		cmd.sniffer_cfg.phy_info_mode =
			cpu_to_le32(ndev->type == ARPHRD_IEEE80211_RADIOTAP);
		cmd.sniffer_cfg.phy_support =
			cpu_to_le32((wil->monitor_flags & MONITOR_FLAG_CONTROL)
				    ? WMI_SNIFFER_CP : WMI_SNIFFER_BOTH_PHYS);
	} else {
		/* Initialize offload (in non-sniffer mode).
		 * Linux IP stack always calculates IP checksum
		 * HW always calculate TCP/UDP checksum
		 */
		cmd.l3_l4_ctrl |= (1 << L3_L4_CTRL_TCPIP_CHECKSUM_EN_POS);
	}

	if (rx_align_2)
		cmd.l2_802_3_offload_ctrl |=
				L2_802_3_OFFLOAD_CTRL_SNAP_KEEP_MSK;

	/* typical time for secure PCP is 840ms */
	rc = wmi_call(wil, WMI_CFG_RX_CHAIN_CMDID, &cmd, sizeof(cmd),
		      WMI_CFG_RX_CHAIN_DONE_EVENTID, &evt, sizeof(evt), 2000);
	if (rc)
		return rc;

	vring->hwtail = le32_to_cpu(evt.evt.rx_ring_tail_ptr);

	wil_dbg_misc(wil, "Rx init: status %d tail 0x%08x\n",
		     le32_to_cpu(evt.evt.status), vring->hwtail);

	if (le32_to_cpu(evt.evt.status) != WMI_CFG_RX_CHAIN_SUCCESS)
		rc = -EINVAL;

	return rc;
}