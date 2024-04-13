static int sony_register_touchpad(struct sony_sc *sc, int touch_count,
		int w, int h, int touch_major, int touch_minor, int orientation)
{
	size_t name_sz;
	char *name;
	int ret;

	sc->touchpad = devm_input_allocate_device(&sc->hdev->dev);
	if (!sc->touchpad)
		return -ENOMEM;

	input_set_drvdata(sc->touchpad, sc);
	sc->touchpad->dev.parent = &sc->hdev->dev;
	sc->touchpad->phys = sc->hdev->phys;
	sc->touchpad->uniq = sc->hdev->uniq;
	sc->touchpad->id.bustype = sc->hdev->bus;
	sc->touchpad->id.vendor = sc->hdev->vendor;
	sc->touchpad->id.product = sc->hdev->product;
	sc->touchpad->id.version = sc->hdev->version;

	/* Append a suffix to the controller name as there are various
	 * DS4 compatible non-Sony devices with different names.
	 */
	name_sz = strlen(sc->hdev->name) + sizeof(DS4_TOUCHPAD_SUFFIX);
	name = devm_kzalloc(&sc->hdev->dev, name_sz, GFP_KERNEL);
	if (!name)
		return -ENOMEM;
	snprintf(name, name_sz, "%s" DS4_TOUCHPAD_SUFFIX, sc->hdev->name);
	sc->touchpad->name = name;

	/* We map the button underneath the touchpad to BTN_LEFT. */
	__set_bit(EV_KEY, sc->touchpad->evbit);
	__set_bit(BTN_LEFT, sc->touchpad->keybit);
	__set_bit(INPUT_PROP_BUTTONPAD, sc->touchpad->propbit);

	input_set_abs_params(sc->touchpad, ABS_MT_POSITION_X, 0, w, 0, 0);
	input_set_abs_params(sc->touchpad, ABS_MT_POSITION_Y, 0, h, 0, 0);

	if (touch_major > 0) {
		input_set_abs_params(sc->touchpad, ABS_MT_TOUCH_MAJOR, 
			0, touch_major, 0, 0);
		if (touch_minor > 0)
			input_set_abs_params(sc->touchpad, ABS_MT_TOUCH_MINOR, 
				0, touch_minor, 0, 0);
		if (orientation > 0)
			input_set_abs_params(sc->touchpad, ABS_MT_ORIENTATION, 
				0, orientation, 0, 0);
	}

	if (sc->quirks & NSG_MRXU_REMOTE) {
		__set_bit(EV_REL, sc->touchpad->evbit);
	}

	ret = input_mt_init_slots(sc->touchpad, touch_count, INPUT_MT_POINTER);
	if (ret < 0)
		return ret;

	ret = input_register_device(sc->touchpad);
	if (ret < 0)
		return ret;

	return 0;
}