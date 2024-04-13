static int ms_input_mapping(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct ms_data *ms = hid_get_drvdata(hdev);
	unsigned long quirks = ms->quirks;

	if (quirks & MS_ERGONOMY) {
		int ret = ms_ergonomy_kb_quirk(hi, usage, bit, max);
		if (ret)
			return ret;
	}

	if ((quirks & MS_PRESENTER) &&
			ms_presenter_8k_quirk(hi, usage, bit, max))
		return 1;

	if (quirks & MS_SURFACE_DIAL) {
		int ret = ms_surface_dial_quirk(hi, field, usage, bit, max);

		if (ret)
			return ret;
	}

	return 0;
}