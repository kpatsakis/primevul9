static void ms_ff_worker(struct work_struct *work)
{
	struct ms_data *ms = container_of(work, struct ms_data, ff_worker);
	struct hid_device *hdev = ms->hdev;
	struct xb1s_ff_report *r = ms->output_report_dmabuf;
	int ret;

	memset(r, 0, sizeof(*r));

	r->report_id = XB1S_FF_REPORT;
	r->enable = ENABLE_WEAK | ENABLE_STRONG;
	/*
	 * Specifying maximum duration and maximum loop count should
	 * cover maximum duration of a single effect, which is 65536
	 * ms
	 */
	r->duration_10ms = U8_MAX;
	r->loop_count = U8_MAX;
	r->magnitude[MAGNITUDE_STRONG] = ms->strong; /* left actuator */
	r->magnitude[MAGNITUDE_WEAK] = ms->weak;     /* right actuator */

	ret = hid_hw_output_report(hdev, (__u8 *)r, sizeof(*r));
	if (ret < 0)
		hid_warn(hdev, "failed to send FF report\n");
}