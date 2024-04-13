static int dualshock4_get_version_info(struct sony_sc *sc)
{
	u8 *buf;
	int ret;

	buf = kmalloc(DS4_FEATURE_REPORT_0xA3_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(sc->hdev, 0xA3, buf,
				 DS4_FEATURE_REPORT_0xA3_SIZE,
				 HID_FEATURE_REPORT,
				 HID_REQ_GET_REPORT);
	if (ret < 0) {
		kfree(buf);
		return ret;
	}

	sc->hw_version = get_unaligned_le16(&buf[35]);
	sc->fw_version = get_unaligned_le16(&buf[41]);

	kfree(buf);
	return 0;
}