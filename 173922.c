static void motion_send_output_report(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	struct motion_output_report_02 *report =
		(struct motion_output_report_02 *)sc->output_report_dmabuf;

	memset(report, 0, MOTION_REPORT_0x02_SIZE);

	report->type = 0x02; /* set leds */
	report->r = sc->led_state[0];
	report->g = sc->led_state[1];
	report->b = sc->led_state[2];

#ifdef CONFIG_SONY_FF
	report->rumble = max(sc->right, sc->left);
#endif

	hid_hw_output_report(hdev, (u8 *)report, MOTION_REPORT_0x02_SIZE);
}