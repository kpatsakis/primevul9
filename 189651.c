int wmi_get_temperature(struct wil6210_priv *wil, u32 *t_bb, u32 *t_rf)
{
	int rc;
	struct wmi_temp_sense_cmd cmd = {
		.measure_baseband_en = cpu_to_le32(!!t_bb),
		.measure_rf_en = cpu_to_le32(!!t_rf),
		.measure_mode = cpu_to_le32(TEMPERATURE_MEASURE_NOW),
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_temp_sense_done_event evt;
	} __packed reply;

	rc = wmi_call(wil, WMI_TEMP_SENSE_CMDID, &cmd, sizeof(cmd),
		      WMI_TEMP_SENSE_DONE_EVENTID, &reply, sizeof(reply), 100);
	if (rc)
		return rc;

	if (t_bb)
		*t_bb = le32_to_cpu(reply.evt.baseband_t1000);
	if (t_rf)
		*t_rf = le32_to_cpu(reply.evt.rf_t1000);

	return 0;
}