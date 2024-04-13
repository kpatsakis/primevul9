static void lg4ff_set_range_g25(struct hid_device *hid, u16 range)
{
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
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
	dbg_hid("G25/G27/DFGT: setting range to %u\n", range);

	spin_lock_irqsave(&entry->report_lock, flags);
	value[0] = 0xf8;
	value[1] = 0x81;
	value[2] = range & 0x00ff;
	value[3] = (range & 0xff00) >> 8;
	value[4] = 0x00;
	value[5] = 0x00;
	value[6] = 0x00;

	hid_hw_request(hid, entry->report, HID_REQ_SET_REPORT);
	spin_unlock_irqrestore(&entry->report_lock, flags);
}