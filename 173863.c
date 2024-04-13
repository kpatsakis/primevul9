static int hidpp_devicenametype_get_device_name(struct hidpp_device *hidpp,
	u8 feature_index, u8 char_index, char *device_name, int len_buf)
{
	struct hidpp_report response;
	int ret, i;
	int count;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
		CMD_GET_DEVICE_NAME_TYPE_GET_DEVICE_NAME, &char_index, 1,
		&response);

	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	switch (response.report_id) {
	case REPORT_ID_HIDPP_VERY_LONG:
		count = hidpp->very_long_report_length - 4;
		break;
	case REPORT_ID_HIDPP_LONG:
		count = HIDPP_REPORT_LONG_LENGTH - 4;
		break;
	case REPORT_ID_HIDPP_SHORT:
		count = HIDPP_REPORT_SHORT_LENGTH - 4;
		break;
	default:
		return -EPROTO;
	}

	if (len_buf < count)
		count = len_buf;

	for (i = 0; i < count; i++)
		device_name[i] = response.fap.params[i];

	return count;
}