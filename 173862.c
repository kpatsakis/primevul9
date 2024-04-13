static void ms_remove_ff(struct hid_device *hdev)
{
	struct ms_data *ms = hid_get_drvdata(hdev);

	if (!(ms->quirks & MS_QUIRK_FF))
		return;

	cancel_work_sync(&ms->ff_worker);
}