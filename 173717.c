static char *hidpp_unifying_get_name(struct hidpp_device *hidpp_dev)
{
	struct hidpp_report response;
	int ret;
	u8 params[1] = { HIDPP_DEVICE_NAME };
	char *name;
	int len;

	ret = hidpp_send_rap_command_sync(hidpp_dev,
					REPORT_ID_HIDPP_SHORT,
					HIDPP_GET_LONG_REGISTER,
					HIDPP_REG_PAIRING_INFORMATION,
					params, 1, &response);
	if (ret)
		return NULL;

	len = response.rap.params[1];

	if (2 + len > sizeof(response.rap.params))
		return NULL;

	if (len < 4) /* logitech devices are usually at least Xddd */
		return NULL;

	name = kzalloc(len + 1, GFP_KERNEL);
	if (!name)
		return NULL;

	memcpy(name, &response.rap.params[2], len);

	/* include the terminating '\0' */
	hidpp_prefix_name(&name, len + 1);

	return name;
}