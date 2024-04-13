static int xc2028_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int rc;

	tuner_dbg("%s called\n", __func__);

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	*frequency = priv->frequency;

	return 0;
}