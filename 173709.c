static int hidpp_input_configured(struct hid_device *hdev,
				struct hid_input *hidinput)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);
	struct input_dev *input = hidinput->input;

	if (!hidpp)
		return 0;

	hidpp_populate_input(hidpp, input);

	return 0;
}