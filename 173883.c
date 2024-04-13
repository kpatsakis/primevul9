static void lg4ff_set_leds(struct hid_device *hid, u8 leds)
{
	struct lg_drv_data *drv_data;
	struct lg4ff_device_entry *entry;
	unsigned long flags;
	s32 *value;

	drv_data = hid_get_drvdata(hid);
	if (!drv_data) {
		hid_err(hid, "Private driver data not found!\n");
		return;
	}

	entry = drv_data->device_props;
	if (!entry) {
		hid_err(hid, "Device properties not found!\n");
		return;
	}
	value = entry->report->field[0]->value;

	spin_lock_irqsave(&entry->report_lock, flags);
	value[0] = 0xf8;
	value[1] = 0x12;
	value[2] = leds;
	value[3] = 0x00;
	value[4] = 0x00;
	value[5] = 0x00;
	value[6] = 0x00;
	hid_hw_request(hid, entry->report, HID_REQ_SET_REPORT);
	spin_unlock_irqrestore(&entry->report_lock, flags);
}