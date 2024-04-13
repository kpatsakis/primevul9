static ssize_t ds4_show_poll_interval(struct device *dev,
				struct device_attribute
				*attr, char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);

	return snprintf(buf, PAGE_SIZE, "%i\n", sc->ds4_bt_poll_interval);
}