static int xc2028_dvb_release(struct dvb_frontend *fe)
{
	struct xc2028_data *priv = fe->tuner_priv;

	tuner_dbg("%s called\n", __func__);

	mutex_lock(&xc2028_list_mutex);

	/* only perform final cleanup if this is the last instance */
	if (hybrid_tuner_report_instance_count(priv) == 1) {
		free_firmware(priv);
		kfree(priv->ctrl.fname);
		priv->ctrl.fname = NULL;
	}

	if (priv)
		hybrid_tuner_release_state(priv);

	mutex_unlock(&xc2028_list_mutex);

	fe->tuner_priv = NULL;

	return 0;
}