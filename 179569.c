static int af9005_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msg[],
			   int num)
{
	/* only implements what the mt2060 module does, don't know how
	   to make it really generic */
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	int ret;
	u8 reg, addr;
	u8 *value;

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	if (num > 2)
		warn("more than 2 i2c messages at a time is not handled yet. TODO.");

	if (num == 2) {
		/* reads a single register */
		reg = *msg[0].buf;
		addr = msg[0].addr;
		value = msg[1].buf;
		ret = af9005_i2c_read(d, addr, reg, value, 1);
		if (ret == 0)
			ret = 2;
	} else {
		/* write one or more registers */
		reg = msg[0].buf[0];
		addr = msg[0].addr;
		value = &msg[0].buf[1];
		ret = af9005_i2c_write(d, addr, reg, value, msg[0].len - 1);
		if (ret == 0)
			ret = 1;
	}

	mutex_unlock(&d->i2c_mutex);
	return ret;
}