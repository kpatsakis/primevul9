static ssize_t lg4ff_combine_store(struct device *dev, struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct hid_device *hid = to_hid_device(dev);
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	u16 combine = simple_strtoul(buf, NULL, 10);

	drv_data = hid_get_drvdata(hid);
	if (!drv_data) {
		hid_err(hid, "Private driver data not found!\n");
		return -EINVAL;
	}

	entry = drv_data->device_props;
	if (!entry) {
		hid_err(hid, "Device properties not found!\n");
		return -EINVAL;
	}

	if (combine > 1)
		combine = 1;

	entry->wdata.combine = combine;
	return count;
}