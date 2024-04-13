static void wtp_populate_input(struct hidpp_device *hidpp,
			       struct input_dev *input_dev)
{
	struct wtp_data *wd = hidpp->private_data;

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(EV_KEY, input_dev->evbit);
	__clear_bit(EV_REL, input_dev->evbit);
	__clear_bit(EV_LED, input_dev->evbit);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, wd->x_size, 0, 0);
	input_abs_set_res(input_dev, ABS_MT_POSITION_X, wd->resolution);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, wd->y_size, 0, 0);
	input_abs_set_res(input_dev, ABS_MT_POSITION_Y, wd->resolution);

	/* Max pressure is not given by the devices, pick one */
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 50, 0, 0);

	input_set_capability(input_dev, EV_KEY, BTN_LEFT);

	if (hidpp->quirks & HIDPP_QUIRK_WTP_PHYSICAL_BUTTONS)
		input_set_capability(input_dev, EV_KEY, BTN_RIGHT);
	else
		__set_bit(INPUT_PROP_BUTTONPAD, input_dev->propbit);

	input_mt_init_slots(input_dev, wd->maxcontacts, INPUT_MT_POINTER |
		INPUT_MT_DROP_UNUSED);
}