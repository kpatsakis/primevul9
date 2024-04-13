static void hog_device_accept(struct hog_device *dev, struct gatt_db *db)
{
	char name[248];
	uint16_t vendor, product, version;

	if (dev->hog)
		return;

	if (device_name_known(dev->device))
		device_get_name(dev->device, name, sizeof(name));
	else
		strcpy(name, "bluez-hog-device");

	vendor = btd_device_get_vendor(dev->device);
	product = btd_device_get_product(dev->device);
	version = btd_device_get_version(dev->device);

	DBG("name=%s vendor=0x%X, product=0x%X, version=0x%X", name, vendor,
							product, version);

	dev->hog = bt_hog_new_default(name, vendor, product, version, db);
}