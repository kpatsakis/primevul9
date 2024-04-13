int i2400m_radio_is(struct i2400m *i2400m, enum wimax_rf_state state)
{
	if (state == WIMAX_RF_OFF)
		return i2400m->state == I2400M_SS_RF_OFF
			|| i2400m->state == I2400M_SS_RF_SHUTDOWN;
	else if (state == WIMAX_RF_ON)
		/* state == WIMAX_RF_ON */
		return i2400m->state != I2400M_SS_RF_OFF
			&& i2400m->state != I2400M_SS_RF_SHUTDOWN;
	else {
		BUG();
		return -EINVAL;	/* shut gcc warnings on certain arches */
	}
}