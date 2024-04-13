static int hidpp_send_fap_command_sync(struct hidpp_device *hidpp,
	u8 feat_index, u8 funcindex_clientid, u8 *params, int param_count,
	struct hidpp_report *response)
{
	struct hidpp_report *message;
	int ret;

	if (param_count > sizeof(message->fap.params))
		return -EINVAL;

	message = kzalloc(sizeof(struct hidpp_report), GFP_KERNEL);
	if (!message)
		return -ENOMEM;

	if (param_count > (HIDPP_REPORT_LONG_LENGTH - 4))
		message->report_id = REPORT_ID_HIDPP_VERY_LONG;
	else
		message->report_id = REPORT_ID_HIDPP_LONG;
	message->fap.feature_index = feat_index;
	message->fap.funcindex_clientid = funcindex_clientid;
	memcpy(&message->fap.params, params, param_count);

	ret = hidpp_send_message_sync(hidpp, message, response);
	kfree(message);
	return ret;
}