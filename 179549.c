static int af9005_usb_read_tuner_registers(struct dvb_usb_device *d,
					   u16 reg, u8 * values, int len)
{
	return af9005_generic_read_write(d, reg,
					 AF9005_CMD_READ, AF9005_TUNER_REG,
					 values, len);
}