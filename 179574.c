static int af9005_boot_packet(struct usb_device *udev, int type, u8 *reply,
			      u8 *buf, int size)
{
	u16 checksum;
	int act_len, i, ret;

	memset(buf, 0, size);
	buf[0] = (u8) (FW_BULKOUT_SIZE & 0xff);
	buf[1] = (u8) ((FW_BULKOUT_SIZE >> 8) & 0xff);
	switch (type) {
	case FW_CONFIG:
		buf[2] = 0x11;
		buf[3] = 0x04;
		buf[4] = 0x00;	/* sequence number, original driver doesn't increment it here */
		buf[5] = 0x03;
		checksum = buf[4] + buf[5];
		buf[6] = (u8) ((checksum >> 8) & 0xff);
		buf[7] = (u8) (checksum & 0xff);
		break;
	case FW_CONFIRM:
		buf[2] = 0x11;
		buf[3] = 0x04;
		buf[4] = 0x00;	/* sequence number, original driver doesn't increment it here */
		buf[5] = 0x01;
		checksum = buf[4] + buf[5];
		buf[6] = (u8) ((checksum >> 8) & 0xff);
		buf[7] = (u8) (checksum & 0xff);
		break;
	case FW_BOOT:
		buf[2] = 0x10;
		buf[3] = 0x08;
		buf[4] = 0x00;	/* sequence number, original driver doesn't increment it here */
		buf[5] = 0x97;
		buf[6] = 0xaa;
		buf[7] = 0x55;
		buf[8] = 0xa5;
		buf[9] = 0x5a;
		checksum = 0;
		for (i = 4; i <= 9; i++)
			checksum += buf[i];
		buf[10] = (u8) ((checksum >> 8) & 0xff);
		buf[11] = (u8) (checksum & 0xff);
		break;
	default:
		err("boot packet invalid boot packet type");
		return -EINVAL;
	}
	deb_fw(">>> ");
	debug_dump(buf, FW_BULKOUT_SIZE + 2, deb_fw);

	ret = usb_bulk_msg(udev,
			   usb_sndbulkpipe(udev, 0x02),
			   buf, FW_BULKOUT_SIZE + 2, &act_len, 2000);
	if (ret)
		err("boot packet bulk message failed: %d (%d/%d)", ret,
		    FW_BULKOUT_SIZE + 2, act_len);
	else
		ret = act_len != FW_BULKOUT_SIZE + 2 ? -1 : 0;
	if (ret)
		return ret;
	memset(buf, 0, 9);
	ret = usb_bulk_msg(udev,
			   usb_rcvbulkpipe(udev, 0x01), buf, 9, &act_len, 2000);
	if (ret) {
		err("boot packet recv bulk message failed: %d", ret);
		return ret;
	}
	deb_fw("<<< ");
	debug_dump(buf, act_len, deb_fw);
	checksum = 0;
	switch (type) {
	case FW_CONFIG:
		if (buf[2] != 0x11) {
			err("boot bad config header.");
			return -EIO;
		}
		if (buf[3] != 0x05) {
			err("boot bad config size.");
			return -EIO;
		}
		if (buf[4] != 0x00) {
			err("boot bad config sequence.");
			return -EIO;
		}
		if (buf[5] != 0x04) {
			err("boot bad config subtype.");
			return -EIO;
		}
		for (i = 4; i <= 6; i++)
			checksum += buf[i];
		if (buf[7] * 256 + buf[8] != checksum) {
			err("boot bad config checksum.");
			return -EIO;
		}
		*reply = buf[6];
		break;
	case FW_CONFIRM:
		if (buf[2] != 0x11) {
			err("boot bad confirm header.");
			return -EIO;
		}
		if (buf[3] != 0x05) {
			err("boot bad confirm size.");
			return -EIO;
		}
		if (buf[4] != 0x00) {
			err("boot bad confirm sequence.");
			return -EIO;
		}
		if (buf[5] != 0x02) {
			err("boot bad confirm subtype.");
			return -EIO;
		}
		for (i = 4; i <= 6; i++)
			checksum += buf[i];
		if (buf[7] * 256 + buf[8] != checksum) {
			err("boot bad confirm checksum.");
			return -EIO;
		}
		*reply = buf[6];
		break;
	case FW_BOOT:
		if (buf[2] != 0x10) {
			err("boot bad boot header.");
			return -EIO;
		}
		if (buf[3] != 0x05) {
			err("boot bad boot size.");
			return -EIO;
		}
		if (buf[4] != 0x00) {
			err("boot bad boot sequence.");
			return -EIO;
		}
		if (buf[5] != 0x01) {
			err("boot bad boot pattern 01.");
			return -EIO;
		}
		if (buf[6] != 0x10) {
			err("boot bad boot pattern 10.");
			return -EIO;
		}
		for (i = 4; i <= 6; i++)
			checksum += buf[i];
		if (buf[7] * 256 + buf[8] != checksum) {
			err("boot bad boot checksum.");
			return -EIO;
		}
		break;

	}

	return 0;
}