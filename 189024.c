static int seek_firmware(struct dvb_frontend *fe, unsigned int type,
			 v4l2_std_id *id)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int                 i, best_i = -1, best_nr_matches = 0;
	unsigned int        type_mask = 0;

	tuner_dbg("%s called, want type=", __func__);
	if (debug) {
		dump_firm_type(type);
		printk("(%x), id %016llx.\n", type, (unsigned long long)*id);
	}

	if (!priv->firm) {
		tuner_err("Error! firmware not loaded\n");
		return -EINVAL;
	}

	if (((type & ~SCODE) == 0) && (*id == 0))
		*id = V4L2_STD_PAL;

	if (type & BASE)
		type_mask = BASE_TYPES;
	else if (type & SCODE) {
		type &= SCODE_TYPES;
		type_mask = SCODE_TYPES & ~HAS_IF;
	} else if (type & DTV_TYPES)
		type_mask = DTV_TYPES;
	else if (type & STD_SPECIFIC_TYPES)
		type_mask = STD_SPECIFIC_TYPES;

	type &= type_mask;

	if (!(type & SCODE))
		type_mask = ~0;

	/* Seek for exact match */
	for (i = 0; i < priv->firm_size; i++) {
		if ((type == (priv->firm[i].type & type_mask)) &&
		    (*id == priv->firm[i].id))
			goto found;
	}

	/* Seek for generic video standard match */
	for (i = 0; i < priv->firm_size; i++) {
		v4l2_std_id match_mask;
		int nr_matches;

		if (type != (priv->firm[i].type & type_mask))
			continue;

		match_mask = *id & priv->firm[i].id;
		if (!match_mask)
			continue;

		if ((*id & match_mask) == *id)
			goto found; /* Supports all the requested standards */

		nr_matches = hweight64(match_mask);
		if (nr_matches > best_nr_matches) {
			best_nr_matches = nr_matches;
			best_i = i;
		}
	}

	if (best_nr_matches > 0) {
		tuner_dbg("Selecting best matching firmware (%d bits) for "
			  "type=", best_nr_matches);
		dump_firm_type(type);
		printk("(%x), id %016llx:\n", type, (unsigned long long)*id);
		i = best_i;
		goto found;
	}

	/*FIXME: Would make sense to seek for type "hint" match ? */

	i = -ENOENT;
	goto ret;

found:
	*id = priv->firm[i].id;

ret:
	tuner_dbg("%s firmware for type=", (i < 0) ? "Can't find" : "Found");
	if (debug) {
		dump_firm_type(type);
		printk("(%x), id %016llx.\n", type, (unsigned long long)*id);
	}
	return i;
}