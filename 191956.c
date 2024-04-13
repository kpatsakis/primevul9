void rtl8xxxu_update_rate_mask(struct rtl8xxxu_priv *priv, u32 ramask, int sgi)
{
	struct h2c_cmd h2c;

	memset(&h2c, 0, sizeof(struct h2c_cmd));

	h2c.ramask.cmd = H2C_SET_RATE_MASK;
	h2c.ramask.mask_lo = cpu_to_le16(ramask & 0xffff);
	h2c.ramask.mask_hi = cpu_to_le16(ramask >> 16);

	h2c.ramask.arg = 0x80;
	if (sgi)
		h2c.ramask.arg |= 0x20;

	dev_dbg(&priv->udev->dev, "%s: rate mask %08x, arg %02x, size %zi\n",
		__func__, ramask, h2c.ramask.arg, sizeof(h2c.ramask));
	rtl8xxxu_gen1_h2c_cmd(priv, &h2c, sizeof(h2c.ramask));
}