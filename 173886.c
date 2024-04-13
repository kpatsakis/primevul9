static void ms_remove(struct hid_device *hdev)
{
	hid_hw_stop(hdev);
	ms_remove_ff(hdev);
}