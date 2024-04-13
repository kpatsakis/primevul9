static int load_scode(struct dvb_frontend *fe, unsigned int type,
			 v4l2_std_id *id, __u16 int_freq, int scode)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int                pos, rc;
	unsigned char	   *p;

	tuner_dbg("%s called\n", __func__);

	if (!int_freq) {
		pos = seek_firmware(fe, type, id);
		if (pos < 0)
			return pos;
	} else {
		for (pos = 0; pos < priv->firm_size; pos++) {
			if ((priv->firm[pos].int_freq == int_freq) &&
			    (priv->firm[pos].type & HAS_IF))
				break;
		}
		if (pos == priv->firm_size)
			return -ENOENT;
	}

	p = priv->firm[pos].ptr;

	if (priv->firm[pos].type & HAS_IF) {
		if (priv->firm[pos].size != 12 * 16 || scode >= 16)
			return -EINVAL;
		p += 12 * scode;
	} else {
		/* 16 SCODE entries per file; each SCODE entry is 12 bytes and
		 * has a 2-byte size header in the firmware format. */
		if (priv->firm[pos].size != 14 * 16 || scode >= 16 ||
		    le16_to_cpu(*(__le16 *)(p + 14 * scode)) != 12)
			return -EINVAL;
		p += 14 * scode + 2;
	}

	tuner_info("Loading SCODE for type=");
	dump_firm_type_and_int_freq(priv->firm[pos].type,
				    priv->firm[pos].int_freq);
	printk("(%x), id %016llx.\n", priv->firm[pos].type,
	       (unsigned long long)*id);

	if (priv->firm_version < 0x0202)
		rc = send_seq(priv, {0x20, 0x00, 0x00, 0x00});
	else
		rc = send_seq(priv, {0xa0, 0x00, 0x00, 0x00});
	if (rc < 0)
		return -EIO;

	rc = i2c_send(priv, p, 12);
	if (rc < 0)
		return -EIO;

	rc = send_seq(priv, {0x00, 0x8c});
	if (rc < 0)
		return -EIO;

	return 0;
}