static int ath6kl_wmi_startscan_cmd(struct wmi *wmi, u8 if_idx,
				    enum wmi_scan_type scan_type,
				    u32 force_fgscan, u32 is_legacy,
				    u32 home_dwell_time,
				    u32 force_scan_interval,
				    s8 num_chan, u16 *ch_list)
{
	struct sk_buff *skb;
	struct wmi_start_scan_cmd *sc;
	s8 size;
	int i, ret;

	size = sizeof(struct wmi_start_scan_cmd);

	if ((scan_type != WMI_LONG_SCAN) && (scan_type != WMI_SHORT_SCAN))
		return -EINVAL;

	if (num_chan > WMI_MAX_CHANNELS)
		return -EINVAL;

	if (num_chan)
		size += sizeof(u16) * (num_chan - 1);

	skb = ath6kl_wmi_get_new_buf(size);
	if (!skb)
		return -ENOMEM;

	sc = (struct wmi_start_scan_cmd *) skb->data;
	sc->scan_type = scan_type;
	sc->force_fg_scan = cpu_to_le32(force_fgscan);
	sc->is_legacy = cpu_to_le32(is_legacy);
	sc->home_dwell_time = cpu_to_le32(home_dwell_time);
	sc->force_scan_intvl = cpu_to_le32(force_scan_interval);
	sc->num_ch = num_chan;

	for (i = 0; i < num_chan; i++)
		sc->ch_list[i] = cpu_to_le16(ch_list[i]);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_START_SCAN_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}