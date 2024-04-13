static int m560_send_config_command(struct hid_device *hdev, bool connected)
{
	struct hidpp_report response;
	struct hidpp_device *hidpp_dev;

	hidpp_dev = hid_get_drvdata(hdev);

	return hidpp_send_rap_command_sync(
		hidpp_dev,
		REPORT_ID_HIDPP_SHORT,
		M560_SUB_ID,
		M560_BUTTON_MODE_REGISTER,
		(u8 *)m560_config_parameter,
		sizeof(m560_config_parameter),
		&response
	);
}