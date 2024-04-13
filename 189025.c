static int xc2028_get_afc(struct dvb_frontend *fe, s32 *afc)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int i, rc;
	u16 frq_lock = 0;
	s16 afc_reg = 0;

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	/* If the device is sleeping, no channel is tuned */
	if (!rc) {
		*afc = 0;
		return 0;
	}

	mutex_lock(&priv->lock);

	/* Sync Lock Indicator */
	for (i = 0; i < 3; i++) {
		rc = xc2028_get_reg(priv, XREG_LOCK, &frq_lock);
		if (rc < 0)
			goto ret;

		if (frq_lock)
			break;
		msleep(6);
	}

	/* Frequency didn't lock */
	if (frq_lock == 2)
		goto ret;

	/* Get AFC */
	rc = xc2028_get_reg(priv, XREG_FREQ_ERROR, &afc_reg);
	if (rc < 0)
		goto ret;

	*afc = afc_reg * 15625; /* Hz */

	tuner_dbg("AFC is %d Hz\n", *afc);

ret:
	mutex_unlock(&priv->lock);

	return rc;
}