static int ath6kl_wmi_bitrate_reply_rx(struct wmi *wmi, u8 *datap, int len)
{
	struct wmi_bit_rate_reply *reply;
	s32 rate;
	u32 sgi, index;

	if (len < sizeof(struct wmi_bit_rate_reply))
		return -EINVAL;

	reply = (struct wmi_bit_rate_reply *) datap;

	ath6kl_dbg(ATH6KL_DBG_WMI, "rateindex %d\n", reply->rate_index);

	if (reply->rate_index == (s8) RATE_AUTO) {
		rate = RATE_AUTO;
	} else {
		index = reply->rate_index & 0x7f;
		if (WARN_ON_ONCE(index > (RATE_MCS_7_40 + 1)))
			return -EINVAL;

		sgi = (reply->rate_index & 0x80) ? 1 : 0;
		rate = wmi_rate_tbl[index][sgi];
	}

	ath6kl_wakeup_event(wmi->parent_dev);

	return 0;
}