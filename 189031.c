static int xc2028_signal(struct dvb_frontend *fe, u16 *strength)
{
	struct xc2028_data *priv = fe->tuner_priv;
	u16                 frq_lock, signal = 0;
	int                 rc, i;

	tuner_dbg("%s called\n", __func__);

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	/* If the device is sleeping, no channel is tuned */
	if (!rc) {
		*strength = 0;
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

	/* Get SNR of the video signal */
	rc = xc2028_get_reg(priv, XREG_SNR, &signal);
	if (rc < 0)
		goto ret;

	/* Signal level is 3 bits only */

	signal = ((1 << 12) - 1) | ((signal & 0x07) << 12);

ret:
	mutex_unlock(&priv->lock);

	*strength = signal;

	tuner_dbg("signal strength is %d\n", signal);

	return rc;
}