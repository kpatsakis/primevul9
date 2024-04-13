static ssize_t hidpp_ff_range_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct hid_device *hid = to_hid_device(dev);
	struct hid_input *hidinput = list_entry(hid->inputs.next, struct hid_input, list);
	struct input_dev *idev = hidinput->input;
	struct hidpp_ff_private_data *data = idev->ff->private;

	return scnprintf(buf, PAGE_SIZE, "%u\n", data->range);
}