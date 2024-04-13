int af9005_read_ofdm_registers(struct dvb_usb_device *d, u16 reg,
			       u8 * values, int len)
{
	int ret;
	deb_reg("read %d registers %x ", len, reg);
	ret = af9005_generic_read_write(d, reg,
					AF9005_CMD_READ, AF9005_OFDM_REG,
					values, len);
	if (ret)
		deb_reg("failed\n");
	else
		debug_dump(values, len, deb_reg);
	return ret;
}