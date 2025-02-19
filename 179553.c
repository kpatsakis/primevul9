static int af9005_download_firmware(struct usb_device *udev, const struct firmware *fw)
{
	int i, packets, ret, act_len;

	u8 *buf;
	u8 reply;

	buf = kmalloc(FW_BULKOUT_SIZE + 2, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = af9005_boot_packet(udev, FW_CONFIG, &reply, buf,
				 FW_BULKOUT_SIZE + 2);
	if (ret)
		goto err;
	if (reply != 0x01) {
		err("before downloading firmware, FW_CONFIG expected 0x01, received 0x%x", reply);
		ret = -EIO;
		goto err;
	}
	packets = fw->size / FW_BULKOUT_SIZE;
	buf[0] = (u8) (FW_BULKOUT_SIZE & 0xff);
	buf[1] = (u8) ((FW_BULKOUT_SIZE >> 8) & 0xff);
	for (i = 0; i < packets; i++) {
		memcpy(&buf[2], fw->data + i * FW_BULKOUT_SIZE,
		       FW_BULKOUT_SIZE);
		deb_fw(">>> ");
		debug_dump(buf, FW_BULKOUT_SIZE + 2, deb_fw);
		ret = usb_bulk_msg(udev,
				   usb_sndbulkpipe(udev, 0x02),
				   buf, FW_BULKOUT_SIZE + 2, &act_len, 1000);
		if (ret) {
			err("firmware download failed at packet %d with code %d", i, ret);
			goto err;
		}
	}
	ret = af9005_boot_packet(udev, FW_CONFIRM, &reply,
				 buf, FW_BULKOUT_SIZE + 2);
	if (ret)
		goto err;
	if (reply != (u8) (packets & 0xff)) {
		err("after downloading firmware, FW_CONFIRM expected 0x%x, received 0x%x", packets & 0xff, reply);
		ret = -EIO;
		goto err;
	}
	ret = af9005_boot_packet(udev, FW_BOOT, &reply, buf,
				 FW_BULKOUT_SIZE + 2);
	if (ret)
		goto err;
	ret = af9005_boot_packet(udev, FW_CONFIG, &reply, buf,
				 FW_BULKOUT_SIZE + 2);
	if (ret)
		goto err;
	if (reply != 0x02) {
		err("after downloading firmware, FW_CONFIG expected 0x02, received 0x%x", reply);
		ret = -EIO;
		goto err;
	}

err:
	kfree(buf);
	return ret;

}