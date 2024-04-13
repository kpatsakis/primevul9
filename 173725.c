static int hi_res_scroll_enable(struct hidpp_device *hidpp)
{
	int ret;
	u8 multiplier = 1;

	if (hidpp->quirks & HIDPP_QUIRK_HI_RES_SCROLL_X2121) {
		ret = hidpp_hrw_set_wheel_mode(hidpp, false, true, false);
		if (ret == 0)
			ret = hidpp_hrw_get_wheel_capability(hidpp, &multiplier);
	} else if (hidpp->quirks & HIDPP_QUIRK_HI_RES_SCROLL_X2120) {
		ret = hidpp_hrs_set_highres_scrolling_mode(hidpp, true,
							   &multiplier);
	} else /* if (hidpp->quirks & HIDPP_QUIRK_HI_RES_SCROLL_1P0) */ {
		ret = hidpp10_enable_scrolling_acceleration(hidpp);
		multiplier = 8;
	}
	if (ret)
		return ret;

	if (multiplier == 0)
		multiplier = 1;

	hidpp->vertical_wheel_counter.wheel_multiplier = multiplier;
	hid_info(hidpp->hid_dev, "multiplier = %d\n", multiplier);
	return 0;
}