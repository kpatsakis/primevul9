void i2400m_report_tlv_rf_switches_status(
	struct i2400m *i2400m,
	const struct i2400m_tlv_rf_switches_status *rfss)
{
	struct device *dev = i2400m_dev(i2400m);
	enum i2400m_rf_switch_status hw, sw;
	enum wimax_st wimax_state;

	sw = le32_to_cpu(rfss->sw_rf_switch);
	hw = le32_to_cpu(rfss->hw_rf_switch);

	d_fnstart(3, dev, "(i2400m %p rfss %p [hw %u sw %u])\n",
		  i2400m, rfss, hw, sw);
	/* We only process rw switch evens when the device has been
	 * fully initialized */
	wimax_state = wimax_state_get(&i2400m->wimax_dev);
	if (wimax_state < WIMAX_ST_RADIO_OFF) {
		d_printf(3, dev, "ignoring RF switches report, state %u\n",
			 wimax_state);
		goto out;
	}
	switch (sw) {
	case I2400M_RF_SWITCH_ON:	/* RF Kill disabled (radio on) */
		wimax_report_rfkill_sw(&i2400m->wimax_dev, WIMAX_RF_ON);
		break;
	case I2400M_RF_SWITCH_OFF:	/* RF Kill enabled (radio off) */
		wimax_report_rfkill_sw(&i2400m->wimax_dev, WIMAX_RF_OFF);
		break;
	default:
		dev_err(dev, "HW BUG? Unknown RF SW state 0x%x\n", sw);
	}

	switch (hw) {
	case I2400M_RF_SWITCH_ON:	/* RF Kill disabled (radio on) */
		wimax_report_rfkill_hw(&i2400m->wimax_dev, WIMAX_RF_ON);
		break;
	case I2400M_RF_SWITCH_OFF:	/* RF Kill enabled (radio off) */
		wimax_report_rfkill_hw(&i2400m->wimax_dev, WIMAX_RF_OFF);
		break;
	default:
		dev_err(dev, "HW BUG? Unknown RF HW state 0x%x\n", hw);
	}
out:
	d_fnend(3, dev, "(i2400m %p rfss %p [hw %u sw %u]) = void\n",
		i2400m, rfss, hw, sw);
}