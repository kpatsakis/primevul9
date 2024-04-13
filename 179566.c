static int __init af9005_usb_module_init(void)
{
	int result;
	if ((result = usb_register(&af9005_usb_driver))) {
		err("usb_register failed. (%d)", result);
		return result;
	}
#if IS_MODULE(CONFIG_DVB_USB_AF9005) || defined(CONFIG_DVB_USB_AF9005_REMOTE)
	/* FIXME: convert to todays kernel IR infrastructure */
	rc_decode = symbol_request(af9005_rc_decode);
	rc_keys = symbol_request(rc_map_af9005_table);
	rc_keys_size = symbol_request(rc_map_af9005_table_size);
#endif
	if (rc_decode == NULL || rc_keys == NULL || rc_keys_size == NULL) {
		err("af9005_rc_decode function not found, disabling remote");
		af9005_properties.rc.legacy.rc_query = NULL;
	} else {
		af9005_properties.rc.legacy.rc_map_table = rc_keys;
		af9005_properties.rc.legacy.rc_map_size = *rc_keys_size;
	}

	return 0;
}