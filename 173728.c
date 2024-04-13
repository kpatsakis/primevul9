static int hidpp_send_rap_command_sync(struct hidpp_device *hidpp_dev,
	u8 report_id, u8 sub_id, u8 reg_address, u8 *params, int param_count,
	struct hidpp_report *response)
{
	struct hidpp_report *message;
	int ret, max_count;

	switch (report_id) {
	case REPORT_ID_HIDPP_SHORT:
		max_count = HIDPP_REPORT_SHORT_LENGTH - 4;
		break;
	case REPORT_ID_HIDPP_LONG:
		max_count = HIDPP_REPORT_LONG_LENGTH - 4;
		break;
	case REPORT_ID_HIDPP_VERY_LONG:
		max_count = hidpp_dev->very_long_report_length - 4;
		break;
	default:
		return -EINVAL;
	}

	if (param_count > max_count)
		return -EINVAL;

	message = kzalloc(sizeof(struct hidpp_report), GFP_KERNEL);
	if (!message)
		return -ENOMEM;
	message->report_id = report_id;
	message->rap.sub_id = sub_id;
	message->rap.reg_address = reg_address;
	memcpy(&message->rap.params, params, param_count);

	ret = hidpp_send_message_sync(hidpp_dev, message, response);
	kfree(message);
	return ret;
}