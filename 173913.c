static ssize_t sony_show_hardware_version(struct device *dev,
				struct device_attribute
				*attr, char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);

	return snprintf(buf, PAGE_SIZE, "0x%04x\n", sc->hw_version);
}