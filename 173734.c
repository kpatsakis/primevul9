int lgff_init(struct hid_device* hid)
{
	struct hid_input *hidinput;
	struct input_dev *dev;
	const signed short *ff_bits = ff_joystick;
	int error;
	int i;

	if (list_empty(&hid->inputs)) {
		hid_err(hid, "no inputs found\n");
		return -ENODEV;
	}
	hidinput = list_entry(hid->inputs.next, struct hid_input, list);
	dev = hidinput->input;

	/* Check that the report looks ok */
	if (!hid_validate_values(hid, HID_OUTPUT_REPORT, 0, 0, 7))
		return -ENODEV;

	for (i = 0; i < ARRAY_SIZE(devices); i++) {
		if (dev->id.vendor == devices[i].idVendor &&
		    dev->id.product == devices[i].idProduct) {
			ff_bits = devices[i].ff;
			break;
		}
	}

	for (i = 0; ff_bits[i] >= 0; i++)
		set_bit(ff_bits[i], dev->ffbit);

	error = input_ff_create_memless(dev, NULL, hid_lgff_play);
	if (error)
		return error;

	if ( test_bit(FF_AUTOCENTER, dev->ffbit) )
		dev->ff->set_autocenter = hid_lgff_set_autocenter;

	pr_info("Force feedback for Logitech force feedback devices by Johann Deneux <johann.deneux@it.uu.se>\n");

	return 0;
}