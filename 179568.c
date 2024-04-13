int af9005_read_ofdm_register(struct dvb_usb_device *d, u16 reg, u8 * value)
{
	int ret;
	deb_reg("read register %x ", reg);
	ret = af9005_generic_read_write(d, reg,
					AF9005_CMD_READ, AF9005_OFDM_REG,
					value, 1);
	if (ret)
		deb_reg("failed\n");
	else
		deb_reg("value %x\n", *value);
	return ret;
}