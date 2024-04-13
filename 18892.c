void hid_dump_report(struct hid_device *hid, int type, u8 *data,
		int size)
{
	struct hid_report_enum *report_enum;
	char *buf;
	unsigned int i;

	buf = kmalloc(sizeof(char) * HID_DEBUG_BUFSIZE, GFP_ATOMIC);

	if (!buf)
		return;

	report_enum = hid->report_enum + type;

	/* dump the report */
	snprintf(buf, HID_DEBUG_BUFSIZE - 1,
			"\nreport (size %u) (%snumbered) = ", size,
			report_enum->numbered ? "" : "un");
	hid_debug_event(hid, buf);

	for (i = 0; i < size; i++) {
		snprintf(buf, HID_DEBUG_BUFSIZE - 1,
				" %02x", data[i]);
		hid_debug_event(hid, buf);
	}
	hid_debug_event(hid, "\n");
	kfree(buf);
}