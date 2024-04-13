struct dvb_frontend *xc2028_attach(struct dvb_frontend *fe,
				   struct xc2028_config *cfg)
{
	struct xc2028_data *priv;
	int instance;

	if (debug)
		printk(KERN_DEBUG "xc2028: Xcv2028/3028 init called!\n");

	if (NULL == cfg)
		return NULL;

	if (!fe) {
		printk(KERN_ERR "xc2028: No frontend!\n");
		return NULL;
	}

	mutex_lock(&xc2028_list_mutex);

	instance = hybrid_tuner_request_state(struct xc2028_data, priv,
					      hybrid_tuner_instance_list,
					      cfg->i2c_adap, cfg->i2c_addr,
					      "xc2028");
	switch (instance) {
	case 0:
		/* memory allocation failure */
		goto fail;
	case 1:
		/* new tuner instance */
		priv->ctrl.max_len = 13;

		mutex_init(&priv->lock);

		fe->tuner_priv = priv;
		break;
	case 2:
		/* existing tuner instance */
		fe->tuner_priv = priv;
		break;
	}

	memcpy(&fe->ops.tuner_ops, &xc2028_dvb_tuner_ops,
	       sizeof(xc2028_dvb_tuner_ops));

	tuner_info("type set to %s\n", "XCeive xc2028/xc3028 tuner");

	if (cfg->ctrl)
		xc2028_set_config(fe, cfg->ctrl);

	mutex_unlock(&xc2028_list_mutex);

	return fe;
fail:
	mutex_unlock(&xc2028_list_mutex);

	xc2028_dvb_release(fe);
	return NULL;
}