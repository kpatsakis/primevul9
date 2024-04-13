static int emsff_play(struct input_dev *dev, void *data,
			 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct emsff_device *emsff = data;
	int weak, strong;

	weak = effect->u.rumble.weak_magnitude;
	strong = effect->u.rumble.strong_magnitude;

	dbg_hid("called with 0x%04x 0x%04x\n", strong, weak);

	weak = weak * 0xff / 0xffff;
	strong = strong * 0xff / 0xffff;

	emsff->report->field[0]->value[1] = weak;
	emsff->report->field[0]->value[2] = strong;

	dbg_hid("running with 0x%02x 0x%02x\n", strong, weak);
	hid_hw_request(hid, emsff->report, HID_REQ_SET_REPORT);

	return 0;
}