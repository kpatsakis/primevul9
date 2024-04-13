static int hidpp10_set_register(struct hidpp_device *hidpp_dev,
	u8 register_address, u8 byte, u8 mask, u8 value)
{
	struct hidpp_report response;
	int ret;
	u8 params[3] = { 0 };

	ret = hidpp_send_rap_command_sync(hidpp_dev,
					  REPORT_ID_HIDPP_SHORT,
					  HIDPP_GET_REGISTER,
					  register_address,
					  NULL, 0, &response);
	if (ret)
		return ret;

	memcpy(params, response.rap.params, 3);

	params[byte] &= ~mask;
	params[byte] |= value & mask;

	return hidpp_send_rap_command_sync(hidpp_dev,
					   REPORT_ID_HIDPP_SHORT,
					   HIDPP_SET_REGISTER,
					   register_address,
					   params, 3, &response);
}