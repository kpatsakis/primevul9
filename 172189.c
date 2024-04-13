static int hidp_set_raw_report(struct hid_device *hid, unsigned char reportnum,
			       unsigned char *data, size_t count,
			       unsigned char report_type)
{
	struct hidp_session *session = hid->driver_data;
	int ret;

	switch (report_type) {
	case HID_FEATURE_REPORT:
		report_type = HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_FEATURE;
		break;
	case HID_INPUT_REPORT:
		report_type = HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_INPUT;
		break;
	case HID_OUTPUT_REPORT:
		report_type = HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_OUPUT;
		break;
	default:
		return -EINVAL;
	}

	if (mutex_lock_interruptible(&session->report_mutex))
		return -ERESTARTSYS;

	/* Set up our wait, and send the report request to the device. */
	data[0] = reportnum;
	set_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags);
	ret = hidp_send_ctrl_message(session, report_type, data, count);
	if (ret)
		goto err;

	/* Wait for the ACK from the device. */
	while (test_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags) &&
	       !atomic_read(&session->terminate)) {
		int res;

		res = wait_event_interruptible_timeout(session->report_queue,
			!test_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags)
				|| atomic_read(&session->terminate),
			10*HZ);
		if (res == 0) {
			/* timeout */
			ret = -EIO;
			goto err;
		}
		if (res < 0) {
			/* signal */
			ret = -ERESTARTSYS;
			goto err;
		}
	}

	if (!session->output_report_success) {
		ret = -EIO;
		goto err;
	}

	ret = count;

err:
	clear_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags);
	mutex_unlock(&session->report_mutex);
	return ret;
}