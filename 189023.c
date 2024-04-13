static int xc2028_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 delsys = c->delivery_system;
	u32 bw = c->bandwidth_hz;
	struct xc2028_data *priv = fe->tuner_priv;
	int rc;
	unsigned int       type = 0;
	u16                demod = 0;

	tuner_dbg("%s called\n", __func__);

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	switch (delsys) {
	case SYS_DVBT:
	case SYS_DVBT2:
		/*
		 * The only countries with 6MHz seem to be Taiwan/Uruguay.
		 * Both seem to require QAM firmware for OFDM decoding
		 * Tested in Taiwan by Terry Wu <terrywu2009@gmail.com>
		 */
		if (bw <= 6000000)
			type |= QAM;

		switch (priv->ctrl.type) {
		case XC2028_D2633:
			type |= D2633;
			break;
		case XC2028_D2620:
			type |= D2620;
			break;
		case XC2028_AUTO:
		default:
			/* Zarlink seems to need D2633 */
			if (priv->ctrl.demod == XC3028_FE_ZARLINK456)
				type |= D2633;
			else
				type |= D2620;
		}
		break;
	case SYS_ATSC:
		/* The only ATSC firmware (at least on v2.7) is D2633 */
		type |= ATSC | D2633;
		break;
	/* DVB-S and pure QAM (FE_QAM) are not supported */
	default:
		return -EINVAL;
	}

	if (bw <= 6000000) {
		type |= DTV6;
		priv->ctrl.vhfbw7 = 0;
		priv->ctrl.uhfbw8 = 0;
	} else if (bw <= 7000000) {
		if (c->frequency < 470000000)
			priv->ctrl.vhfbw7 = 1;
		else
			priv->ctrl.uhfbw8 = 0;
		type |= (priv->ctrl.vhfbw7 && priv->ctrl.uhfbw8) ? DTV78 : DTV7;
		type |= F8MHZ;
	} else {
		if (c->frequency < 470000000)
			priv->ctrl.vhfbw7 = 0;
		else
			priv->ctrl.uhfbw8 = 1;
		type |= (priv->ctrl.vhfbw7 && priv->ctrl.uhfbw8) ? DTV78 : DTV8;
		type |= F8MHZ;
	}

	/* All S-code tables need a 200kHz shift */
	if (priv->ctrl.demod) {
		demod = priv->ctrl.demod;

		/*
		 * Newer firmwares require a 200 kHz offset only for ATSC
		 */
		if (type == ATSC || priv->firm_version < 0x0302)
			demod += 200;
		/*
		 * The DTV7 S-code table needs a 700 kHz shift.
		 *
		 * DTV7 is only used in Australia.  Germany or Italy may also
		 * use this firmware after initialization, but a tune to a UHF
		 * channel should then cause DTV78 to be used.
		 *
		 * Unfortunately, on real-field tests, the s-code offset
		 * didn't work as expected, as reported by
		 * Robert Lowery <rglowery@exemail.com.au>
		 */
	}

	return generic_set_freq(fe, c->frequency,
				V4L2_TUNER_DIGITAL_TV, type, 0, demod);
}