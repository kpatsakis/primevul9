int lg4ff_adjust_input_event(struct hid_device *hid, struct hid_field *field,
			     struct hid_usage *usage, s32 value, struct lg_drv_data *drv_data)
{
	struct lg4ff_device_entry *entry = drv_data->device_props;
	s32 new_value = 0;

	if (!entry) {
		hid_err(hid, "Device properties not found");
		return 0;
	}

	switch (entry->wdata.product_id) {
	case USB_DEVICE_ID_LOGITECH_DFP_WHEEL:
		switch (usage->code) {
		case ABS_X:
			new_value = lg4ff_adjust_dfp_x_axis(value, entry->wdata.range);
			input_event(field->hidinput->input, usage->type, usage->code, new_value);
			return 1;
		default:
			return 0;
		}
	default:
		return 0;
	}
}