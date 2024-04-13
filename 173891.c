static void ax_remove(struct hid_device *hdev)
{
	hid_hw_close(hdev);
	hid_hw_stop(hdev);
}