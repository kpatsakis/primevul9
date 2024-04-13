int af9005_write_ofdm_register(struct dvb_usb_device *d, u16 reg, u8 value)
{
	int ret;
	u8 temp = value;
	deb_reg("write register %x value %x ", reg, value);
	ret = af9005_generic_read_write(d, reg,
					AF9005_CMD_WRITE, AF9005_OFDM_REG,
					&temp, 1);
	if (ret)
		deb_reg("failed\n");
	else
		deb_reg("ok\n");
	return ret;
}