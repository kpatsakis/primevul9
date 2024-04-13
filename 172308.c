static void hog_remove(struct btd_service *service)
{
	struct hog_device *dev = btd_service_get_user_data(service);
	struct btd_device *device = btd_service_get_device(service);
	const char *path = device_get_path(device);

	DBG("path %s", path);

	hog_device_free(dev);
}