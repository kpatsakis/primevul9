static void hidpp_populate_input(struct hidpp_device *hidpp,
				 struct input_dev *input)
{
	hidpp->input = input;

	if (hidpp->quirks & HIDPP_QUIRK_CLASS_WTP)
		wtp_populate_input(hidpp, input);
	else if (hidpp->quirks & HIDPP_QUIRK_CLASS_M560)
		m560_populate_input(hidpp, input);

	if (hidpp->quirks & HIDPP_QUIRK_HIDPP_WHEELS)
		hidpp10_wheel_populate_input(hidpp, input);

	if (hidpp->quirks & HIDPP_QUIRK_HIDPP_EXTRA_MOUSE_BTNS)
		hidpp10_extra_mouse_buttons_populate_input(hidpp, input);
}