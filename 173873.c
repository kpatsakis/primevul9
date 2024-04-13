static int hidpp_ff_init(struct hidpp_device *hidpp, u8 feature_index)
{
	struct hid_device *hid = hidpp->hid_dev;
	struct hid_input *hidinput;
	struct input_dev *dev;
	const struct usb_device_descriptor *udesc = &(hid_to_usb_dev(hid)->descriptor);
	const u16 bcdDevice = le16_to_cpu(udesc->bcdDevice);
	struct ff_device *ff;
	struct hidpp_report response;
	struct hidpp_ff_private_data *data;
	int error, j, num_slots;
	u8 version;

	if (list_empty(&hid->inputs)) {
		hid_err(hid, "no inputs found\n");
		return -ENODEV;
	}
	hidinput = list_entry(hid->inputs.next, struct hid_input, list);
	dev = hidinput->input;

	if (!dev) {
		hid_err(hid, "Struct input_dev not set!\n");
		return -EINVAL;
	}

	/* Get firmware release */
	version = bcdDevice & 255;

	/* Set supported force feedback capabilities */
	for (j = 0; hidpp_ff_effects[j] >= 0; j++)
		set_bit(hidpp_ff_effects[j], dev->ffbit);
	if (version > 1)
		for (j = 0; hidpp_ff_effects_v2[j] >= 0; j++)
			set_bit(hidpp_ff_effects_v2[j], dev->ffbit);

	/* Read number of slots available in device */
	error = hidpp_send_fap_command_sync(hidpp, feature_index,
		HIDPP_FF_GET_INFO, NULL, 0, &response);
	if (error) {
		if (error < 0)
			return error;
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, error);
		return -EPROTO;
	}

	num_slots = response.fap.params[0] - HIDPP_FF_RESERVED_SLOTS;

	error = input_ff_create(dev, num_slots);

	if (error) {
		hid_err(dev, "Failed to create FF device!\n");
		return error;
	}

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	data->effect_ids = kcalloc(num_slots, sizeof(int), GFP_KERNEL);
	if (!data->effect_ids) {
		kfree(data);
		return -ENOMEM;
	}
	data->wq = create_singlethread_workqueue("hidpp-ff-sendqueue");
	if (!data->wq) {
		kfree(data->effect_ids);
		kfree(data);
		return -ENOMEM;
	}

	data->hidpp = hidpp;
	data->feature_index = feature_index;
	data->version = version;
	data->slot_autocenter = 0;
	data->num_effects = num_slots;
	for (j = 0; j < num_slots; j++)
		data->effect_ids[j] = -1;

	ff = dev->ff;
	ff->private = data;

	ff->upload = hidpp_ff_upload_effect;
	ff->erase = hidpp_ff_erase_effect;
	ff->playback = hidpp_ff_playback;
	ff->set_gain = hidpp_ff_set_gain;
	ff->set_autocenter = hidpp_ff_set_autocenter;
	ff->destroy = hidpp_ff_destroy;


	/* reset all forces */
	error = hidpp_send_fap_command_sync(hidpp, feature_index,
		HIDPP_FF_RESET_ALL, NULL, 0, &response);

	/* Read current Range */
	error = hidpp_send_fap_command_sync(hidpp, feature_index,
		HIDPP_FF_GET_APERTURE, NULL, 0, &response);
	if (error)
		hid_warn(hidpp->hid_dev, "Failed to read range from device!\n");
	data->range = error ? 900 : get_unaligned_be16(&response.fap.params[0]);

	/* Create sysfs interface */
	error = device_create_file(&(hidpp->hid_dev->dev), &dev_attr_range);
	if (error)
		hid_warn(hidpp->hid_dev, "Unable to create sysfs interface for \"range\", errno %d!\n", error);

	/* Read the current gain values */
	error = hidpp_send_fap_command_sync(hidpp, feature_index,
		HIDPP_FF_GET_GLOBAL_GAINS, NULL, 0, &response);
	if (error)
		hid_warn(hidpp->hid_dev, "Failed to read gain values from device!\n");
	data->gain = error ? 0xffff : get_unaligned_be16(&response.fap.params[0]);
	/* ignore boost value at response.fap.params[2] */

	/* init the hardware command queue */
	atomic_set(&data->workqueue_size, 0);

	/* initialize with zero autocenter to get wheel in usable state */
	hidpp_ff_set_autocenter(dev, 0);

	hid_info(hid, "Force feedback support loaded (firmware release %d).\n",
		 version);

	return 0;
}