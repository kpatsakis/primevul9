static int dr_input_mapping(struct hid_device *hdev, struct hid_input *hi,
			    struct hid_field *field, struct hid_usage *usage,
			    unsigned long **bit, int *max)
{
	switch (usage->hid) {
	/*
	 * revert to the old hid-input behavior where axes
	 * can be randomly assigned when hid->usage is reused.
	 */
	case HID_GD_X: case HID_GD_Y: case HID_GD_Z:
	case HID_GD_RX: case HID_GD_RY: case HID_GD_RZ:
		if (field->flags & HID_MAIN_ITEM_RELATIVE)
			map_rel(usage->hid & 0xf);
		else
			map_abs(usage->hid & 0xf);
		return 1;
	}

	return 0;
}