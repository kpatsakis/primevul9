static ssize_t lg4ff_alternate_modes_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct hid_device *hid = to_hid_device(dev);
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	ssize_t count = 0;
	int i;

	drv_data = hid_get_drvdata(hid);
	if (!drv_data) {
		hid_err(hid, "Private driver data not found!\n");
		return 0;
	}

	entry = drv_data->device_props;
	if (!entry) {
		hid_err(hid, "Device properties not found!\n");
		return 0;
	}

	if (!entry->wdata.real_name) {
		hid_err(hid, "NULL pointer to string\n");
		return 0;
	}

	for (i = 0; i < LG4FF_MODE_MAX_IDX; i++) {
		if (entry->wdata.alternate_modes & BIT(i)) {
			/* Print tag and full name */
			count += scnprintf(buf + count, PAGE_SIZE - count, "%s: %s",
					   lg4ff_alternate_modes[i].tag,
					   !lg4ff_alternate_modes[i].product_id ? entry->wdata.real_name : lg4ff_alternate_modes[i].name);
			if (count >= PAGE_SIZE - 1)
				return count;

			/* Mark the currently active mode with an asterisk */
			if (lg4ff_alternate_modes[i].product_id == entry->wdata.product_id ||
			    (lg4ff_alternate_modes[i].product_id == 0 && entry->wdata.product_id == entry->wdata.real_product_id))
				count += scnprintf(buf + count, PAGE_SIZE - count, " *\n");
			else
				count += scnprintf(buf + count, PAGE_SIZE - count, "\n");

			if (count >= PAGE_SIZE - 1)
				return count;
		}
	}

	return count;
}