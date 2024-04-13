static irqreturn_t wcd9335_slimbus_irq(int irq, void *data)
{
	struct wcd9335_codec *wcd = data;
	unsigned long status = 0;
	int i, j, port_id;
	unsigned int val, int_val = 0;
	irqreturn_t ret = IRQ_NONE;
	bool tx;
	unsigned short reg = 0;

	for (i = WCD9335_SLIM_PGD_PORT_INT_STATUS_RX_0, j = 0;
	     i <= WCD9335_SLIM_PGD_PORT_INT_STATUS_TX_1; i++, j++) {
		regmap_read(wcd->if_regmap, i, &val);
		status |= ((u32)val << (8 * j));
	}

	for_each_set_bit(j, &status, 32) {
		tx = (j >= 16 ? true : false);
		port_id = (tx ? j - 16 : j);
		regmap_read(wcd->if_regmap,
				WCD9335_SLIM_PGD_PORT_INT_RX_SOURCE0 + j, &val);
		if (val) {
			if (!tx)
				reg = WCD9335_SLIM_PGD_PORT_INT_EN0 +
					(port_id / 8);
			else
				reg = WCD9335_SLIM_PGD_PORT_INT_TX_EN0 +
					(port_id / 8);
			regmap_read(
				wcd->if_regmap, reg, &int_val);
			/*
			 * Ignore interrupts for ports for which the
			 * interrupts are not specifically enabled.
			 */
			if (!(int_val & (1 << (port_id % 8))))
				continue;
		}

		if (val & WCD9335_SLIM_IRQ_OVERFLOW)
			dev_err_ratelimited(wcd->dev,
			   "%s: overflow error on %s port %d, value %x\n",
			   __func__, (tx ? "TX" : "RX"), port_id, val);

		if (val & WCD9335_SLIM_IRQ_UNDERFLOW)
			dev_err_ratelimited(wcd->dev,
			   "%s: underflow error on %s port %d, value %x\n",
			   __func__, (tx ? "TX" : "RX"), port_id, val);

		if ((val & WCD9335_SLIM_IRQ_OVERFLOW) ||
			(val & WCD9335_SLIM_IRQ_UNDERFLOW)) {
			if (!tx)
				reg = WCD9335_SLIM_PGD_PORT_INT_EN0 +
					(port_id / 8);
			else
				reg = WCD9335_SLIM_PGD_PORT_INT_TX_EN0 +
					(port_id / 8);
			regmap_read(
				wcd->if_regmap, reg, &int_val);
			if (int_val & (1 << (port_id % 8))) {
				int_val = int_val ^ (1 << (port_id % 8));
				regmap_write(wcd->if_regmap,
					reg, int_val);
			}
		}

		regmap_write(wcd->if_regmap,
				WCD9335_SLIM_PGD_PORT_INT_CLR_RX_0 + (j / 8),
				BIT(j % 8));
		ret = IRQ_HANDLED;
	}

	return ret;
}