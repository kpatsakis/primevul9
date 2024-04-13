static int hidpp_raw_hidpp_event(struct hidpp_device *hidpp, u8 *data,
		int size)
{
	struct hidpp_report *question = hidpp->send_receive_buf;
	struct hidpp_report *answer = hidpp->send_receive_buf;
	struct hidpp_report *report = (struct hidpp_report *)data;
	int ret;

	/*
	 * If the mutex is locked then we have a pending answer from a
	 * previously sent command.
	 */
	if (unlikely(mutex_is_locked(&hidpp->send_mutex))) {
		/*
		 * Check for a correct hidpp20 answer or the corresponding
		 * error
		 */
		if (hidpp_match_answer(question, report) ||
				hidpp_match_error(question, report)) {
			*answer = *report;
			hidpp->answer_available = true;
			wake_up(&hidpp->wait);
			/*
			 * This was an answer to a command that this driver sent
			 * We return 1 to hid-core to avoid forwarding the
			 * command upstream as it has been treated by the driver
			 */

			return 1;
		}
	}

	if (unlikely(hidpp_report_is_connect_event(report))) {
		atomic_set(&hidpp->connected,
				!(report->rap.params[0] & (1 << 6)));
		if (schedule_work(&hidpp->work) == 0)
			dbg_hid("%s: connect event already queued\n", __func__);
		return 1;
	}

	if (hidpp->capabilities & HIDPP_CAPABILITY_HIDPP20_BATTERY) {
		ret = hidpp20_battery_event(hidpp, data, size);
		if (ret != 0)
			return ret;
		ret = hidpp_solar_battery_event(hidpp, data, size);
		if (ret != 0)
			return ret;
	}

	if (hidpp->capabilities & HIDPP_CAPABILITY_HIDPP10_BATTERY) {
		ret = hidpp10_battery_event(hidpp, data, size);
		if (ret != 0)
			return ret;
	}

	if (hidpp->quirks & HIDPP_QUIRK_HIDPP_WHEELS) {
		ret = hidpp10_wheel_raw_event(hidpp, data, size);
		if (ret != 0)
			return ret;
	}

	if (hidpp->quirks & HIDPP_QUIRK_HIDPP_EXTRA_MOUSE_BTNS) {
		ret = hidpp10_extra_mouse_buttons_raw_event(hidpp, data, size);
		if (ret != 0)
			return ret;
	}

	if (hidpp->quirks & HIDPP_QUIRK_HIDPP_CONSUMER_VENDOR_KEYS) {
		ret = hidpp10_consumer_keys_raw_event(hidpp, data, size);
		if (ret != 0)
			return ret;
	}

	return 0;
}