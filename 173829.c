static int g920_get_config(struct hidpp_device *hidpp)
{
	u8 feature_type;
	u8 feature_index;
	int ret;

	/* Find feature and store for later use */
	ret = hidpp_root_get_feature(hidpp, HIDPP_PAGE_G920_FORCE_FEEDBACK,
		&feature_index, &feature_type);
	if (ret)
		return ret;

	ret = hidpp_ff_init(hidpp, feature_index);
	if (ret)
		hid_warn(hidpp->hid_dev, "Unable to initialize force feedback support, errno %d\n",
				ret);

	return 0;
}