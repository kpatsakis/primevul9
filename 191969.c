rtl8xxxu_gen1_h2c_cmd(struct rtl8xxxu_priv *priv, struct h2c_cmd *h2c, int len)
{
	struct device *dev = &priv->udev->dev;
	int mbox_nr, retry, retval = 0;
	int mbox_reg, mbox_ext_reg;
	u8 val8;

	mutex_lock(&priv->h2c_mutex);

	mbox_nr = priv->next_mbox;
	mbox_reg = REG_HMBOX_0 + (mbox_nr * 4);
	mbox_ext_reg = REG_HMBOX_EXT_0 + (mbox_nr * 2);

	/*
	 * MBOX ready?
	 */
	retry = 100;
	do {
		val8 = rtl8xxxu_read8(priv, REG_HMTFR);
		if (!(val8 & BIT(mbox_nr)))
			break;
	} while (retry--);

	if (!retry) {
		dev_info(dev, "%s: Mailbox busy\n", __func__);
		retval = -EBUSY;
		goto error;
	}

	/*
	 * Need to swap as it's being swapped again by rtl8xxxu_write16/32()
	 */
	if (len > sizeof(u32)) {
		rtl8xxxu_write16(priv, mbox_ext_reg, le16_to_cpu(h2c->raw.ext));
		if (rtl8xxxu_debug & RTL8XXXU_DEBUG_H2C)
			dev_info(dev, "H2C_EXT %04x\n",
				 le16_to_cpu(h2c->raw.ext));
	}
	rtl8xxxu_write32(priv, mbox_reg, le32_to_cpu(h2c->raw.data));
	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_H2C)
		dev_info(dev, "H2C %08x\n", le32_to_cpu(h2c->raw.data));

	priv->next_mbox = (mbox_nr + 1) % H2C_MAX_MBOX;

error:
	mutex_unlock(&priv->h2c_mutex);
	return retval;
}