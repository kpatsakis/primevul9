static int xc2028_get_reg(struct xc2028_data *priv, u16 reg, u16 *val)
{
	unsigned char buf[2];
	unsigned char ibuf[2];

	tuner_dbg("%s %04x called\n", __func__, reg);

	buf[0] = reg >> 8;
	buf[1] = (unsigned char) reg;

	if (i2c_send_recv(priv, buf, 2, ibuf, 2) != 2)
		return -EIO;

	*val = (ibuf[1]) | (ibuf[0] << 8);
	return 0;
}