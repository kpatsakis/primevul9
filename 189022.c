static int xc2028_sleep(struct dvb_frontend *fe)
{
	struct xc2028_data *priv = fe->tuner_priv;
	int rc;

	rc = check_device_status(priv);
	if (rc < 0)
		return rc;

	/* Device is already in sleep mode */
	if (!rc)
		return 0;

	/* Avoid firmware reload on slow devices or if PM disabled */
	if (no_poweroff || priv->ctrl.disable_power_mgmt)
		return 0;

	tuner_dbg("Putting xc2028/3028 into poweroff mode.\n");
	if (debug > 1) {
		tuner_dbg("Printing sleep stack trace:\n");
		dump_stack();
	}

	mutex_lock(&priv->lock);

	if (priv->firm_version < 0x0202)
		rc = send_seq(priv, {0x00, XREG_POWER_DOWN, 0x00, 0x00});
	else
		rc = send_seq(priv, {0x80, XREG_POWER_DOWN, 0x00, 0x00});

	if (rc >= 0)
		priv->state = XC2028_SLEEP;

	mutex_unlock(&priv->lock);

	return rc;
}