static ssize_t lg4ff_alternate_modes_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct hid_device *hid = to_hid_device(dev);
	struct lg4ff_device_entry *entry;
	struct lg_drv_data *drv_data;
	const struct lg4ff_compat_mode_switch *s;
	u16 target_product_id = 0;
	int i, ret;
	char *lbuf;

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

	/* Allow \n at the end of the input parameter */
	lbuf = kasprintf(GFP_KERNEL, "%s", buf);
	if (!lbuf)
		return -ENOMEM;

	i = strlen(lbuf);
	if (lbuf[i-1] == '\n') {
		if (i == 1) {
			kfree(lbuf);
			return -EINVAL;
		}
		lbuf[i-1] = '\0';
	}

	for (i = 0; i < LG4FF_MODE_MAX_IDX; i++) {
		const u16 mode_product_id = lg4ff_alternate_modes[i].product_id;
		const char *tag = lg4ff_alternate_modes[i].tag;

		if (entry->wdata.alternate_modes & BIT(i)) {
			if (!strcmp(tag, lbuf)) {
				if (!mode_product_id)
					target_product_id = entry->wdata.real_product_id;
				else
					target_product_id = mode_product_id;
				break;
			}
		}
	}

	if (i == LG4FF_MODE_MAX_IDX) {
		hid_info(hid, "Requested mode \"%s\" is not supported by the device\n", lbuf);
		kfree(lbuf);
		return -EINVAL;
	}
	kfree(lbuf); /* Not needed anymore */

	if (target_product_id == entry->wdata.product_id) /* Nothing to do */
		return count;

	/* Automatic switching has to be disabled for the switch to DF-EX mode to work correctly */
	if (target_product_id == USB_DEVICE_ID_LOGITECH_WHEEL && !lg4ff_no_autoswitch) {
		hid_info(hid, "\"%s\" cannot be switched to \"DF-EX\" mode. Load the \"hid_logitech\" module with \"lg4ff_no_autoswitch=1\" parameter set and try again\n",
			 entry->wdata.real_name);
		return -EINVAL;
	}

	/* Take care of hardware limitations */
	if ((entry->wdata.real_product_id == USB_DEVICE_ID_LOGITECH_DFP_WHEEL || entry->wdata.real_product_id == USB_DEVICE_ID_LOGITECH_G25_WHEEL) &&
	    entry->wdata.product_id > target_product_id) {
		hid_info(hid, "\"%s\" cannot be switched back into \"%s\" mode\n", entry->wdata.real_name, lg4ff_alternate_modes[i].name);
		return -EINVAL;
	}

	s = lg4ff_get_mode_switch_command(entry->wdata.real_product_id, target_product_id);
	if (!s) {
		hid_err(hid, "Invalid target product ID %X\n", target_product_id);
		return -EINVAL;
	}

	ret = lg4ff_switch_compatibility_mode(hid, s);
	return (ret == 0 ? count : ret);
}