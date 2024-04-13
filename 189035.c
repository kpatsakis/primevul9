static int check_firmware(struct dvb_frontend *fe, unsigned int type,
			  v4l2_std_id std, __u16 int_freq)
{
	struct xc2028_data         *priv = fe->tuner_priv;
	struct firmware_properties new_fw;
	int			   rc, retry_count = 0;
	u16			   version, hwmodel;
	v4l2_std_id		   std0;

	tuner_dbg("%s called\n", __func__);

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	if (priv->ctrl.mts && !(type & FM))
		type |= MTS;

retry:
	new_fw.type = type;
	new_fw.id = std;
	new_fw.std_req = std;
	new_fw.scode_table = SCODE | priv->ctrl.scode_table;
	new_fw.scode_nr = 0;
	new_fw.int_freq = int_freq;

	tuner_dbg("checking firmware, user requested type=");
	if (debug) {
		dump_firm_type(new_fw.type);
		printk("(%x), id %016llx, ", new_fw.type,
		       (unsigned long long)new_fw.std_req);
		if (!int_freq) {
			printk("scode_tbl ");
			dump_firm_type(priv->ctrl.scode_table);
			printk("(%x), ", priv->ctrl.scode_table);
		} else
			printk("int_freq %d, ", new_fw.int_freq);
		printk("scode_nr %d\n", new_fw.scode_nr);
	}

	/*
	 * No need to reload base firmware if it matches and if the tuner
	 * is not at sleep mode
	 */
	if ((priv->state == XC2028_ACTIVE) &&
	    (((BASE | new_fw.type) & BASE_TYPES) ==
	    (priv->cur_fw.type & BASE_TYPES))) {
		tuner_dbg("BASE firmware not changed.\n");
		goto skip_base;
	}

	/* Updating BASE - forget about all currently loaded firmware */
	memset(&priv->cur_fw, 0, sizeof(priv->cur_fw));

	/* Reset is needed before loading firmware */
	rc = do_tuner_callback(fe, XC2028_TUNER_RESET, 0);
	if (rc < 0)
		goto fail;

	/* BASE firmwares are all std0 */
	std0 = 0;
	rc = load_firmware(fe, BASE | new_fw.type, &std0);
	if (rc < 0) {
		tuner_err("Error %d while loading base firmware\n",
			  rc);
		goto fail;
	}

	/* Load INIT1, if needed */
	tuner_dbg("Load init1 firmware, if exists\n");

	rc = load_firmware(fe, BASE | INIT1 | new_fw.type, &std0);
	if (rc == -ENOENT)
		rc = load_firmware(fe, (BASE | INIT1 | new_fw.type) & ~F8MHZ,
				   &std0);
	if (rc < 0 && rc != -ENOENT) {
		tuner_err("Error %d while loading init1 firmware\n",
			  rc);
		goto fail;
	}

skip_base:
	/*
	 * No need to reload standard specific firmware if base firmware
	 * was not reloaded and requested video standards have not changed.
	 */
	if (priv->cur_fw.type == (BASE | new_fw.type) &&
	    priv->cur_fw.std_req == std) {
		tuner_dbg("Std-specific firmware already loaded.\n");
		goto skip_std_specific;
	}

	/* Reloading std-specific firmware forces a SCODE update */
	priv->cur_fw.scode_table = 0;

	rc = load_firmware(fe, new_fw.type, &new_fw.id);
	if (rc == -ENOENT)
		rc = load_firmware(fe, new_fw.type & ~F8MHZ, &new_fw.id);

	if (rc < 0)
		goto fail;

skip_std_specific:
	if (priv->cur_fw.scode_table == new_fw.scode_table &&
	    priv->cur_fw.scode_nr == new_fw.scode_nr) {
		tuner_dbg("SCODE firmware already loaded.\n");
		goto check_device;
	}

	if (new_fw.type & FM)
		goto check_device;

	/* Load SCODE firmware, if exists */
	tuner_dbg("Trying to load scode %d\n", new_fw.scode_nr);

	rc = load_scode(fe, new_fw.type | new_fw.scode_table, &new_fw.id,
			new_fw.int_freq, new_fw.scode_nr);

check_device:
	if (xc2028_get_reg(priv, 0x0004, &version) < 0 ||
	    xc2028_get_reg(priv, 0x0008, &hwmodel) < 0) {
		tuner_err("Unable to read tuner registers.\n");
		goto fail;
	}

	tuner_dbg("Device is Xceive %d version %d.%d, "
		  "firmware version %d.%d\n",
		  hwmodel, (version & 0xf000) >> 12, (version & 0xf00) >> 8,
		  (version & 0xf0) >> 4, version & 0xf);


	if (priv->ctrl.read_not_reliable)
		goto read_not_reliable;

	/* Check firmware version against what we downloaded. */
	if (priv->firm_version != ((version & 0xf0) << 4 | (version & 0x0f))) {
		if (!priv->ctrl.read_not_reliable) {
			tuner_err("Incorrect readback of firmware version.\n");
			goto fail;
		} else {
			tuner_err("Returned an incorrect version. However, "
				  "read is not reliable enough. Ignoring it.\n");
			hwmodel = 3028;
		}
	}

	/* Check that the tuner hardware model remains consistent over time. */
	if (priv->hwmodel == 0 && (hwmodel == 2028 || hwmodel == 3028)) {
		priv->hwmodel = hwmodel;
		priv->hwvers  = version & 0xff00;
	} else if (priv->hwmodel == 0 || priv->hwmodel != hwmodel ||
		   priv->hwvers != (version & 0xff00)) {
		tuner_err("Read invalid device hardware information - tuner "
			  "hung?\n");
		goto fail;
	}

read_not_reliable:
	priv->cur_fw = new_fw;

	/*
	 * By setting BASE in cur_fw.type only after successfully loading all
	 * firmwares, we can:
	 * 1. Identify that BASE firmware with type=0 has been loaded;
	 * 2. Tell whether BASE firmware was just changed the next time through.
	 */
	priv->cur_fw.type |= BASE;
	priv->state = XC2028_ACTIVE;

	return 0;

fail:
	priv->state = XC2028_NO_FIRMWARE;

	memset(&priv->cur_fw, 0, sizeof(priv->cur_fw));
	if (retry_count < 8) {
		msleep(50);
		retry_count++;
		tuner_dbg("Retrying firmware load\n");
		goto retry;
	}

	/* Firmware didn't load. Put the device to sleep */
	xc2028_sleep(fe);

	if (rc == -ENOENT)
		rc = -EINVAL;
	return rc;
}