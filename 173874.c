static int ms_input_mapped(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct ms_data *ms = hid_get_drvdata(hdev);
	unsigned long quirks = ms->quirks;

	if (quirks & MS_DUPLICATE_USAGES)
		clear_bit(usage->code, *bit);

	return 0;
}