static int xc2028_set_analog_freq(struct dvb_frontend *fe,
			      struct analog_parameters *p)
{
	struct xc2028_data *priv = fe->tuner_priv;
	unsigned int       type=0;

	tuner_dbg("%s called\n", __func__);

	if (p->mode == V4L2_TUNER_RADIO) {
		type |= FM;
		if (priv->ctrl.input1)
			type |= INPUT1;
		return generic_set_freq(fe, (625l * p->frequency) / 10,
				V4L2_TUNER_RADIO, type, 0, 0);
	}

	/* if std is not defined, choose one */
	if (!p->std)
		p->std = V4L2_STD_MN;

	/* PAL/M, PAL/N, PAL/Nc and NTSC variants should use 6MHz firmware */
	if (!(p->std & V4L2_STD_MN))
		type |= F8MHZ;

	/* Add audio hack to std mask */
	p->std |= parse_audio_std_option();

	return generic_set_freq(fe, 62500l * p->frequency,
				V4L2_TUNER_ANALOG_TV, type, p->std, 0);
}