static ssize_t lg4ff_range_store(struct device *dev, struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct hid_device *hid = to_hid_device(dev);
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	u16 range = simple_strtoul(buf, NULL, 10);

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

	if (range == 0)
		range = entry->wdata.max_range;

	/* Check if the wheel supports range setting
	 * and that the range is within limits for the wheel */
	if (entry->wdata.set_range && range >= entry->wdata.min_range && range <= entry->wdata.max_range) {
		entry->wdata.set_range(hid, range);
		entry->wdata.range = range;
	}

	return count;
}