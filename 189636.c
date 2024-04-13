int wmi_led_cfg(struct wil6210_priv *wil, bool enable)
{
	int rc = 0;
	struct wmi_led_cfg_cmd cmd = {
		.led_mode = enable,
		.id = led_id,
		.slow_blink_cfg.blink_on =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_SLOW].on_ms),
		.slow_blink_cfg.blink_off =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_SLOW].off_ms),
		.medium_blink_cfg.blink_on =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_MED].on_ms),
		.medium_blink_cfg.blink_off =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_MED].off_ms),
		.fast_blink_cfg.blink_on =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_FAST].on_ms),
		.fast_blink_cfg.blink_off =
			cpu_to_le32(led_blink_time[WIL_LED_TIME_FAST].off_ms),
		.led_polarity = led_polarity,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_led_cfg_done_event evt;
	} __packed reply;

	if (led_id == WIL_LED_INVALID_ID)
		goto out;

	if (led_id > WIL_LED_MAX_ID) {
		wil_err(wil, "Invalid led id %d\n", led_id);
		rc = -EINVAL;
		goto out;
	}

	wil_dbg_wmi(wil,
		    "%s led %d\n",
		    enable ? "enabling" : "disabling", led_id);

	rc = wmi_call(wil, WMI_LED_CFG_CMDID, &cmd, sizeof(cmd),
		      WMI_LED_CFG_DONE_EVENTID, &reply, sizeof(reply),
		      100);
	if (rc)
		goto out;

	if (reply.evt.status) {
		wil_err(wil, "led %d cfg failed with status %d\n",
			led_id, le32_to_cpu(reply.evt.status));
		rc = -EINVAL;
	}

out:
	return rc;
}