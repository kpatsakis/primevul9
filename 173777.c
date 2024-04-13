static int hidpp_devicenametype_get_count(struct hidpp_device *hidpp,
	u8 feature_index, u8 *nameLength)
{
	struct hidpp_report response;
	int ret;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
		CMD_GET_DEVICE_NAME_TYPE_GET_COUNT, NULL, 0, &response);

	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	*nameLength = response.fap.params[0];

	return ret;
}