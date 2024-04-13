static u8 ath6kl_wmi_get_upper_threshold(s16 rssi,
					 struct sq_threshold_params *sq_thresh,
					 u32 size)
{
	u32 index;
	u8 threshold = (u8) sq_thresh->upper_threshold[size - 1];

	/* The list is already in sorted order. Get the next lower value */
	for (index = 0; index < size; index++) {
		if (rssi < sq_thresh->upper_threshold[index]) {
			threshold = (u8) sq_thresh->upper_threshold[index];
			break;
		}
	}

	return threshold;
}