int af9005_read_eeprom(struct dvb_usb_device *d, u8 address, u8 * values,
		       int len)
{
	struct af9005_device_state *st = d->priv;
	u8 seq;
	int ret, i;

	mutex_lock(&d->data_mutex);

	memset(st->data, 0, sizeof(st->data));

	st->data[0] = 14;		/* length of rest of packet low */
	st->data[1] = 0;		/* length of rest of packer high */

	st->data[2] = 0x2a;		/* read/write eeprom */

	st->data[3] = 12;		/* size */

	st->data[4] = seq = st->sequence++;

	st->data[5] = 0;		/* read */

	st->data[6] = len;
	st->data[7] = address;
	ret = dvb_usb_generic_rw(d, st->data, 16, st->data, 14, 0);
	if (st->data[2] != 0x2b) {
		err("Read eeprom, invalid reply code");
		ret = -EIO;
	} else if (st->data[3] != 10) {
		err("Read eeprom, invalid reply length");
		ret = -EIO;
	} else if (st->data[4] != seq) {
		err("Read eeprom, wrong sequence in reply ");
		ret = -EIO;
	} else if (st->data[5] != 1) {
		err("Read eeprom, wrong status in reply ");
		ret = -EIO;
	}

	if (!ret) {
		for (i = 0; i < len; i++)
			values[i] = st->data[6 + i];
	}
	mutex_unlock(&d->data_mutex);

	return ret;
}