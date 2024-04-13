static int lg4ff_play(struct input_dev *dev, void *data, struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	unsigned long flags;
	s32 *value;
	int x;

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

#define CLAMP(x) do { if (x < 0) x = 0; else if (x > 0xff) x = 0xff; } while (0)

	switch (effect->type) {
	case FF_CONSTANT:
		x = effect->u.ramp.start_level + 0x80;	/* 0x80 is no force */
		CLAMP(x);

		spin_lock_irqsave(&entry->report_lock, flags);
		if (x == 0x80) {
			/* De-activate force in slot-1*/
			value[0] = 0x13;
			value[1] = 0x00;
			value[2] = 0x00;
			value[3] = 0x00;
			value[4] = 0x00;
			value[5] = 0x00;
			value[6] = 0x00;

			hid_hw_request(hid, entry->report, HID_REQ_SET_REPORT);
			spin_unlock_irqrestore(&entry->report_lock, flags);
			return 0;
		}

		value[0] = 0x11;	/* Slot 1 */
		value[1] = 0x08;
		value[2] = x;
		value[3] = 0x80;
		value[4] = 0x00;
		value[5] = 0x00;
		value[6] = 0x00;

		hid_hw_request(hid, entry->report, HID_REQ_SET_REPORT);
		spin_unlock_irqrestore(&entry->report_lock, flags);
		break;
	}
	return 0;
}