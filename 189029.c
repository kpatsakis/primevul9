static int load_firmware(struct dvb_frontend *fe, unsigned int type,
			 v4l2_std_id *id)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int                pos, rc;
	unsigned char      *p, *endp, buf[MAX_XFER_SIZE];

	if (priv->ctrl.max_len > sizeof(buf))
		priv->ctrl.max_len = sizeof(buf);

	tuner_dbg("%s called\n", __func__);

	pos = seek_firmware(fe, type, id);
	if (pos < 0)
		return pos;

	tuner_info("Loading firmware for type=");
	dump_firm_type(priv->firm[pos].type);
	printk("(%x), id %016llx.\n", priv->firm[pos].type,
	       (unsigned long long)*id);

	p = priv->firm[pos].ptr;
	endp = p + priv->firm[pos].size;

	while (p < endp) {
		__u16 size;

		/* Checks if there's enough bytes to read */
		if (p + sizeof(size) > endp) {
			tuner_err("Firmware chunk size is wrong\n");
			return -EINVAL;
		}

		size = le16_to_cpu(*(__le16 *) p);
		p += sizeof(size);

		if (size == 0xffff)
			return 0;

		if (!size) {
			/* Special callback command received */
			rc = do_tuner_callback(fe, XC2028_TUNER_RESET, 0);
			if (rc < 0) {
				tuner_err("Error at RESET code %d\n",
					   (*p) & 0x7f);
				return -EINVAL;
			}
			continue;
		}
		if (size >= 0xff00) {
			switch (size) {
			case 0xff00:
				rc = do_tuner_callback(fe, XC2028_RESET_CLK, 0);
				if (rc < 0) {
					tuner_err("Error at RESET code %d\n",
						  (*p) & 0x7f);
					return -EINVAL;
				}
				break;
			default:
				tuner_info("Invalid RESET code %d\n",
					   size & 0x7f);
				return -EINVAL;

			}
			continue;
		}

		/* Checks for a sleep command */
		if (size & 0x8000) {
			msleep(size & 0x7fff);
			continue;
		}

		if ((size + p > endp)) {
			tuner_err("missing bytes: need %d, have %d\n",
				   size, (int)(endp - p));
			return -EINVAL;
		}

		buf[0] = *p;
		p++;
		size--;

		/* Sends message chunks */
		while (size > 0) {
			int len = (size < priv->ctrl.max_len - 1) ?
				   size : priv->ctrl.max_len - 1;

			memcpy(buf + 1, p, len);

			rc = i2c_send(priv, buf, len + 1);
			if (rc < 0) {
				tuner_err("%d returned from send\n", rc);
				return -EINVAL;
			}

			p += len;
			size -= len;
		}

		/* silently fail if the frontend doesn't support I2C flush */
		rc = do_tuner_callback(fe, XC2028_I2C_FLUSH, 0);
		if ((rc < 0) && (rc != -EINVAL)) {
			tuner_err("error executing flush: %d\n", rc);
			return rc;
		}
	}
	return 0;
}