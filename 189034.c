static int load_all_firmwares(struct dvb_frontend *fe,
			      const struct firmware *fw)
{
	struct xc2028_data    *priv = fe->tuner_priv;
	const unsigned char   *p, *endp;
	int                   rc = 0;
	int		      n, n_array;
	char		      name[33];

	tuner_dbg("%s called\n", __func__);

	p = fw->data;
	endp = p + fw->size;

	if (fw->size < sizeof(name) - 1 + 2 + 2) {
		tuner_err("Error: firmware file %s has invalid size!\n",
			  priv->fname);
		goto corrupt;
	}

	memcpy(name, p, sizeof(name) - 1);
	name[sizeof(name) - 1] = 0;
	p += sizeof(name) - 1;

	priv->firm_version = get_unaligned_le16(p);
	p += 2;

	n_array = get_unaligned_le16(p);
	p += 2;

	tuner_info("Loading %d firmware images from %s, type: %s, ver %d.%d\n",
		   n_array, priv->fname, name,
		   priv->firm_version >> 8, priv->firm_version & 0xff);

	priv->firm = kcalloc(n_array, sizeof(*priv->firm), GFP_KERNEL);
	if (priv->firm == NULL) {
		tuner_err("Not enough memory to load firmware file.\n");
		rc = -ENOMEM;
		goto err;
	}
	priv->firm_size = n_array;

	n = -1;
	while (p < endp) {
		__u32 type, size;
		v4l2_std_id id;
		__u16 int_freq = 0;

		n++;
		if (n >= n_array) {
			tuner_err("More firmware images in file than "
				  "were expected!\n");
			goto corrupt;
		}

		/* Checks if there's enough bytes to read */
		if (endp - p < sizeof(type) + sizeof(id) + sizeof(size))
			goto header;

		type = get_unaligned_le32(p);
		p += sizeof(type);

		id = get_unaligned_le64(p);
		p += sizeof(id);

		if (type & HAS_IF) {
			int_freq = get_unaligned_le16(p);
			p += sizeof(int_freq);
			if (endp - p < sizeof(size))
				goto header;
		}

		size = get_unaligned_le32(p);
		p += sizeof(size);

		if (!size || size > endp - p) {
			tuner_err("Firmware type ");
			dump_firm_type(type);
			printk("(%x), id %llx is corrupted "
			       "(size=%d, expected %d)\n",
			       type, (unsigned long long)id,
			       (unsigned)(endp - p), size);
			goto corrupt;
		}

		priv->firm[n].ptr = kzalloc(size, GFP_KERNEL);
		if (priv->firm[n].ptr == NULL) {
			tuner_err("Not enough memory to load firmware file.\n");
			rc = -ENOMEM;
			goto err;
		}
		tuner_dbg("Reading firmware type ");
		if (debug) {
			dump_firm_type_and_int_freq(type, int_freq);
			printk("(%x), id %llx, size=%d.\n",
			       type, (unsigned long long)id, size);
		}

		memcpy(priv->firm[n].ptr, p, size);
		priv->firm[n].type = type;
		priv->firm[n].id   = id;
		priv->firm[n].size = size;
		priv->firm[n].int_freq = int_freq;

		p += size;
	}

	if (n + 1 != priv->firm_size) {
		tuner_err("Firmware file is incomplete!\n");
		goto corrupt;
	}

	goto done;

header:
	tuner_err("Firmware header is incomplete!\n");
corrupt:
	rc = -EINVAL;
	tuner_err("Error: firmware file is corrupted!\n");

err:
	tuner_info("Releasing partially loaded firmware file.\n");
	free_firmware(priv);

done:
	if (rc == 0)
		tuner_dbg("Firmware files loaded.\n");
	else
		priv->state = XC2028_NODEV;

	return rc;
}