static int hidpp_root_get_protocol_version(struct hidpp_device *hidpp)
{
	const u8 ping_byte = 0x5a;
	u8 ping_data[3] = { 0, 0, ping_byte };
	struct hidpp_report response;
	int ret;

	ret = hidpp_send_rap_command_sync(hidpp,
			REPORT_ID_HIDPP_SHORT,
			HIDPP_PAGE_ROOT_IDX,
			CMD_ROOT_GET_PROTOCOL_VERSION,
			ping_data, sizeof(ping_data), &response);

	if (ret == HIDPP_ERROR_INVALID_SUBID) {
		hidpp->protocol_major = 1;
		hidpp->protocol_minor = 0;
		goto print_version;
	}

	/* the device might not be connected */
	if (ret == HIDPP_ERROR_RESOURCE_ERROR)
		return -EIO;

	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	if (response.rap.params[2] != ping_byte) {
		hid_err(hidpp->hid_dev, "%s: ping mismatch 0x%02x != 0x%02x\n",
			__func__, response.rap.params[2], ping_byte);
		return -EPROTO;
	}

	hidpp->protocol_major = response.rap.params[0];
	hidpp->protocol_minor = response.rap.params[1];

print_version:
	hid_info(hidpp->hid_dev, "HID++ %u.%u device connected.\n",
		 hidpp->protocol_major, hidpp->protocol_minor);
	return 0;
}