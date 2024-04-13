static inline int do_tuner_callback(struct dvb_frontend *fe, int cmd, int arg)
{
	struct xc2028_data *priv = fe->tuner_priv;

	/* analog side (tuner-core) uses i2c_adap->algo_data.
	 * digital side is not guaranteed to have algo_data defined.
	 *
	 * digital side will always have fe->dvb defined.
	 * analog side (tuner-core) doesn't (yet) define fe->dvb.
	 */

	return (!fe->callback) ? -EINVAL :
		fe->callback(((fe->dvb) && (fe->dvb->priv)) ?
				fe->dvb->priv : priv->i2c_props.adap->algo_data,
			     DVB_FRONTEND_COMPONENT_TUNER, cmd, arg);
}