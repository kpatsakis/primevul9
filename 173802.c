static int lg4ff_switch_compatibility_mode(struct hid_device *hid, const struct lg4ff_compat_mode_switch *s)
{
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	unsigned long flags;
	s32 *value;
	u8 i;

	drv_data = hid_get_drvdata(hid);
	if (!drv_data) {
		hid_err(hid, "Private driver data not found!\n");
		return -EINVAL;
	}

	entry = drv_data->device_props;
	if (!entry) {
		hid_err(hid, "Device properties not found!\n");
		return -EINVAL;
	}
	value = entry->report->field[0]->value;

	spin_lock_irqsave(&entry->report_lock, flags);
	for (i = 0; i < s->cmd_count; i++) {
		u8 j;

		for (j = 0; j < 7; j++)
			value[j] = s->cmd[j + (7*i)];

		hid_hw_request(hid, entry->report, HID_REQ_SET_REPORT);
	}
	spin_unlock_irqrestore(&entry->report_lock, flags);
	hid_hw_wait(hid);
	return 0;
}