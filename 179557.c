int af9005_write_ofdm_registers(struct dvb_usb_device *d, u16 reg,
				u8 * values, int len)
{
	int ret;
	deb_reg("write %d registers %x values ", len, reg);
	debug_dump(values, len, deb_reg);

	ret = af9005_generic_read_write(d, reg,
					AF9005_CMD_WRITE, AF9005_OFDM_REG,
					values, len);
	if (ret)
		deb_reg("failed\n");
	else
		deb_reg("ok\n");
	return ret;
}