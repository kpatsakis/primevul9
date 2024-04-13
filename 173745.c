static int hidpp_send_message_sync(struct hidpp_device *hidpp,
	struct hidpp_report *message,
	struct hidpp_report *response)
{
	int ret;

	mutex_lock(&hidpp->send_mutex);

	hidpp->send_receive_buf = response;
	hidpp->answer_available = false;

	/*
	 * So that we can later validate the answer when it arrives
	 * in hidpp_raw_event
	 */
	*response = *message;

	ret = __hidpp_send_report(hidpp->hid_dev, message);

	if (ret) {
		dbg_hid("__hidpp_send_report returned err: %d\n", ret);
		memset(response, 0, sizeof(struct hidpp_report));
		goto exit;
	}

	if (!wait_event_timeout(hidpp->wait, hidpp->answer_available,
				5*HZ)) {
		dbg_hid("%s:timeout waiting for response\n", __func__);
		memset(response, 0, sizeof(struct hidpp_report));
		ret = -ETIMEDOUT;
	}

	if (response->report_id == REPORT_ID_HIDPP_SHORT &&
	    response->rap.sub_id == HIDPP_ERROR) {
		ret = response->rap.params[1];
		dbg_hid("%s:got hidpp error %02X\n", __func__, ret);
		goto exit;
	}

	if ((response->report_id == REPORT_ID_HIDPP_LONG ||
			response->report_id == REPORT_ID_HIDPP_VERY_LONG) &&
			response->fap.feature_index == HIDPP20_ERROR) {
		ret = response->fap.params[1];
		dbg_hid("%s:got hidpp 2.0 error %02X\n", __func__, ret);
		goto exit;
	}

exit:
	mutex_unlock(&hidpp->send_mutex);
	return ret;

}