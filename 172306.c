static struct hog_device *hog_device_new(struct btd_device *device)
{
	struct hog_device *dev;

	dev = new0(struct hog_device, 1);
	dev->device = btd_device_ref(device);

	if (!devices)
		devices = queue_new();

	queue_push_tail(devices, dev);

	return dev;
}