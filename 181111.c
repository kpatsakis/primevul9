int ath6kl_wmi_implicit_create_pstream(struct wmi *wmi, u8 if_idx,
				       struct sk_buff *skb,
				       u32 layer2_priority, bool wmm_enabled,
				       u8 *ac)
{
	struct wmi_data_hdr *data_hdr;
	struct ath6kl_llc_snap_hdr *llc_hdr;
	struct wmi_create_pstream_cmd cmd;
	u32 meta_size, hdr_size;
	u16 ip_type = IP_ETHERTYPE;
	u8 stream_exist, usr_pri;
	u8 traffic_class = WMM_AC_BE;
	u8 *datap;

	if (WARN_ON(skb == NULL))
		return -EINVAL;

	datap = skb->data;
	data_hdr = (struct wmi_data_hdr *) datap;

	meta_size = ((le16_to_cpu(data_hdr->info2) >> WMI_DATA_HDR_META_SHIFT) &
		     WMI_DATA_HDR_META_MASK) ? WMI_MAX_TX_META_SZ : 0;

	if (!wmm_enabled) {
		/* If WMM is disabled all traffic goes as BE traffic */
		usr_pri = 0;
	} else {
		hdr_size = sizeof(struct ethhdr);

		llc_hdr = (struct ath6kl_llc_snap_hdr *)(datap +
							 sizeof(struct
								wmi_data_hdr) +
							 meta_size + hdr_size);

		if (llc_hdr->eth_type == htons(ip_type)) {
			/*
			 * Extract the endpoint info from the TOS field
			 * in the IP header.
			 */
			usr_pri =
			   ath6kl_wmi_determine_user_priority(((u8 *) llc_hdr) +
					sizeof(struct ath6kl_llc_snap_hdr),
					layer2_priority);
		} else {
			usr_pri = layer2_priority & 0x7;
		}

		/*
		 * Queue the EAPOL frames in the same WMM_AC_VO queue
		 * as that of management frames.
		 */
		if (skb->protocol == cpu_to_be16(ETH_P_PAE))
			usr_pri = WMI_VOICE_USER_PRIORITY;
	}

	/*
	 * workaround for WMM S5
	 *
	 * FIXME: wmi->traffic_class is always 100 so this test doesn't
	 * make sense
	 */
	if ((wmi->traffic_class == WMM_AC_VI) &&
	    ((usr_pri == 5) || (usr_pri == 4)))
		usr_pri = 1;

	/* Convert user priority to traffic class */
	traffic_class = up_to_ac[usr_pri & 0x7];

	wmi_data_hdr_set_up(data_hdr, usr_pri);

	spin_lock_bh(&wmi->lock);
	stream_exist = wmi->fat_pipe_exist;
	spin_unlock_bh(&wmi->lock);

	if (!(stream_exist & (1 << traffic_class))) {
		memset(&cmd, 0, sizeof(cmd));
		cmd.traffic_class = traffic_class;
		cmd.user_pri = usr_pri;
		cmd.inactivity_int =
			cpu_to_le32(WMI_IMPLICIT_PSTREAM_INACTIVITY_INT);
		/* Implicit streams are created with TSID 0xFF */
		cmd.tsid = WMI_IMPLICIT_PSTREAM;
		ath6kl_wmi_create_pstream_cmd(wmi, if_idx, &cmd);
	}

	*ac = traffic_class;

	return 0;
}