static int ax88179_read_cmd_nopm(struct usbnet *dev, u8 cmd, u16 value,
				 u16 index, u16 size, void *data)
{
	int ret;

	if (2 == size) {
		u16 buf;
		ret = __ax88179_read_cmd(dev, cmd, value, index, size, &buf, 1);
		le16_to_cpus(&buf);
		*((u16 *)data) = buf;
	} else if (4 == size) {
		u32 buf;
		ret = __ax88179_read_cmd(dev, cmd, value, index, size, &buf, 1);
		le32_to_cpus(&buf);
		*((u32 *)data) = buf;
	} else {
		ret = __ax88179_read_cmd(dev, cmd, value, index, size, data, 1);
	}

	return ret;
}