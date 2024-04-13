static void rtl8xxxu_set_basic_rates(struct rtl8xxxu_priv *priv, u32 rate_cfg)
{
	u32 val32;
	u8 rate_idx = 0;

	rate_cfg &= RESPONSE_RATE_BITMAP_ALL;

	val32 = rtl8xxxu_read32(priv, REG_RESPONSE_RATE_SET);
	val32 &= ~RESPONSE_RATE_BITMAP_ALL;
	val32 |= rate_cfg;
	rtl8xxxu_write32(priv, REG_RESPONSE_RATE_SET, val32);

	dev_dbg(&priv->udev->dev, "%s: rates %08x\n", __func__,	rate_cfg);

	while (rate_cfg) {
		rate_cfg = (rate_cfg >> 1);
		rate_idx++;
	}
	rtl8xxxu_write8(priv, REG_INIRTS_RATE_SEL, rate_idx);
}