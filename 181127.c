s32 ath6kl_wmi_get_rate(struct wmi *wmi, s8 rate_index)
{
	struct ath6kl *ar = wmi->parent_dev;
	u8 sgi = 0;
	s32 ret;

	if (rate_index == RATE_AUTO)
		return 0;

	/* SGI is stored as the MSB of the rate_index */
	if (rate_index & RATE_INDEX_MSB) {
		rate_index &= RATE_INDEX_WITHOUT_SGI_MASK;
		sgi = 1;
	}

	if (test_bit(ATH6KL_FW_CAPABILITY_RATETABLE_MCS15,
		     ar->fw_capabilities)) {
		if (WARN_ON(rate_index >= ARRAY_SIZE(wmi_rate_tbl_mcs15)))
			return 0;

		ret = wmi_rate_tbl_mcs15[(u32) rate_index][sgi];
	} else {
		if (WARN_ON(rate_index >= ARRAY_SIZE(wmi_rate_tbl)))
			return 0;

		ret = wmi_rate_tbl[(u32) rate_index][sgi];
	}

	return ret;
}