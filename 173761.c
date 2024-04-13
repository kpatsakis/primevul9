static int hidpp_input_open(struct input_dev *dev)
{
	struct hid_device *hid = input_get_drvdata(dev);

	return hid_hw_open(hid);
}