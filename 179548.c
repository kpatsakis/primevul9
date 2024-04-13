int af9005_read_register_bits(struct dvb_usb_device *d, u16 reg, u8 pos,
			      u8 len, u8 * value)
{
	u8 temp;
	int ret;
	deb_reg("read bits %x %x %x", reg, pos, len);
	ret = af9005_read_ofdm_register(d, reg, &temp);
	if (ret) {
		deb_reg(" failed\n");
		return ret;
	}
	*value = (temp >> pos) & regmask[len - 1];
	deb_reg(" value %x\n", *value);
	return 0;

}