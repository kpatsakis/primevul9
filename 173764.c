static int hidpp_ff_deinit(struct hid_device *hid)
{
	struct hid_input *hidinput = list_entry(hid->inputs.next, struct hid_input, list);
	struct input_dev *dev = hidinput->input;
	struct hidpp_ff_private_data *data;

	if (!dev) {
		hid_err(hid, "Struct input_dev not found!\n");
		return -EINVAL;
	}

	hid_info(hid, "Unloading HID++ force feedback.\n");
	data = dev->ff->private;
	if (!data) {
		hid_err(hid, "Private data not found!\n");
		return -EINVAL;
	}

	destroy_workqueue(data->wq);
	device_remove_file(&hid->dev, &dev_attr_range);

	return 0;
}