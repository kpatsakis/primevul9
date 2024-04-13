int ath6kl_wmi_set_bitrate_mask(struct wmi *wmi, u8 if_idx,
				const struct cfg80211_bitrate_mask *mask)
{
	struct ath6kl *ar = wmi->parent_dev;

	if (test_bit(ATH6KL_FW_CAPABILITY_64BIT_RATES,
		     ar->fw_capabilities))
		return ath6kl_set_bitrate_mask64(wmi, if_idx, mask);
	else
		return ath6kl_set_bitrate_mask32(wmi, if_idx, mask);
}