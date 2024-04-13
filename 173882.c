static int k400_disable_tap_to_click(struct hidpp_device *hidpp)
{
	struct k400_private_data *k400 = hidpp->private_data;
	struct hidpp_touchpad_fw_items items = {};
	int ret;
	u8 feature_type;

	if (!k400->feature_index) {
		ret = hidpp_root_get_feature(hidpp,
			HIDPP_PAGE_TOUCHPAD_FW_ITEMS,
			&k400->feature_index, &feature_type);
		if (ret)
			/* means that the device is not powered up */
			return ret;
	}

	ret = hidpp_touchpad_fw_items_set(hidpp, k400->feature_index, &items);
	if (ret)
		return ret;

	return 0;
}