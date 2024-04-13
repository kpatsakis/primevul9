static int hog_disconnect(struct btd_service *service)
{
	struct hog_device *dev = btd_service_get_user_data(service);

	bt_hog_detach(dev->hog);
	bt_hog_unref(dev->hog);
	dev->hog = NULL;

	btd_service_disconnecting_complete(service, 0);

	return 0;
}