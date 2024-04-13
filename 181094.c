int ath6kl_wmi_beginscan_cmd(struct wmi *wmi, u8 if_idx,
			     enum wmi_scan_type scan_type,
			     u32 force_fgscan, u32 is_legacy,
			     u32 home_dwell_time, u32 force_scan_interval,
			     s8 num_chan, u16 *ch_list, u32 no_cck, u32 *rates)
{
	struct ieee80211_supported_band *sband;
	struct sk_buff *skb;
	struct wmi_begin_scan_cmd *sc;
	s8 size, *supp_rates;
	int i, band, ret;
	struct ath6kl *ar = wmi->parent_dev;
	int num_rates;
	u32 ratemask;

	if (!test_bit(ATH6KL_FW_CAPABILITY_STA_P2PDEV_DUPLEX,
		      ar->fw_capabilities)) {
		return ath6kl_wmi_startscan_cmd(wmi, if_idx,
						scan_type, force_fgscan,
						is_legacy, home_dwell_time,
						force_scan_interval,
						num_chan, ch_list);
	}

	size = sizeof(struct wmi_begin_scan_cmd);

	if ((scan_type != WMI_LONG_SCAN) && (scan_type != WMI_SHORT_SCAN))
		return -EINVAL;

	if (num_chan > WMI_MAX_CHANNELS)
		return -EINVAL;

	if (num_chan)
		size += sizeof(u16) * (num_chan - 1);

	skb = ath6kl_wmi_get_new_buf(size);
	if (!skb)
		return -ENOMEM;

	sc = (struct wmi_begin_scan_cmd *) skb->data;
	sc->scan_type = scan_type;
	sc->force_fg_scan = cpu_to_le32(force_fgscan);
	sc->is_legacy = cpu_to_le32(is_legacy);
	sc->home_dwell_time = cpu_to_le32(home_dwell_time);
	sc->force_scan_intvl = cpu_to_le32(force_scan_interval);
	sc->no_cck = cpu_to_le32(no_cck);
	sc->num_ch = num_chan;

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = ar->wiphy->bands[band];

		if (!sband)
			continue;

		if (WARN_ON(band >= ATH6KL_NUM_BANDS))
			break;

		ratemask = rates[band];
		supp_rates = sc->supp_rates[band].rates;
		num_rates = 0;

		for (i = 0; i < sband->n_bitrates; i++) {
			if ((BIT(i) & ratemask) == 0)
				continue; /* skip rate */
			supp_rates[num_rates++] =
			    (u8) (sband->bitrates[i].bitrate / 5);
		}
		sc->supp_rates[band].nrates = num_rates;
	}

	for (i = 0; i < num_chan; i++)
		sc->ch_list[i] = cpu_to_le16(ch_list[i]);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_BEGIN_SCAN_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}