static int hog_probe(struct btd_service *service)
{
	struct btd_device *device = btd_service_get_device(service);
	const char *path = device_get_path(device);
	struct hog_device *dev;

	DBG("path %s", path);

	dev = hog_device_new(device);
	if (!dev)
		return -EINVAL;

	btd_service_set_user_data(service, dev);
	return 0;
}