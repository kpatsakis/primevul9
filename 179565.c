static int af9005_i2c_read(struct dvb_usb_device *d, u8 i2caddr, u8 reg,
			   u8 * data, int len)
{
	int ret, i;
	u8 temp;
	deb_i2c("i2c_read i2caddr %x, reg %x, len %d\n ", i2caddr, reg, len);
	for (i = 0; i < len; i++) {
		temp = reg + i;
		ret =
		    af9005_read_tuner_registers(d,
						APO_REG_I2C_RW_SILICON_TUNER,
						i2caddr, &temp, 1);
		if (ret) {
			deb_i2c("i2c_read failed\n");
			return ret;
		}
		data[i] = temp;
	}
	deb_i2c("i2c data read: ");
	debug_dump(data, len, deb_i2c);
	return 0;
}