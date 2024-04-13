static void hog_device_free(void *data)
{
	struct hog_device *dev = data;

	queue_remove(devices, dev);
	if (queue_isempty(devices)) {
		queue_destroy(devices, NULL);
		devices = NULL;
	}

	btd_device_unref(dev->device);
	bt_hog_unref(dev->hog);
	free(dev);
}