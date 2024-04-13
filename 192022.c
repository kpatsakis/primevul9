void rtl8xxxu_gen2_update_rate_mask(struct rtl8xxxu_priv *priv,
				    u32 ramask, int sgi)
{
	struct h2c_cmd h2c;
	u8 bw = 0;

	memset(&h2c, 0, sizeof(struct h2c_cmd));

	h2c.b_macid_cfg.cmd = H2C_8723B_MACID_CFG_RAID;
	h2c.b_macid_cfg.ramask0 = ramask & 0xff;
	h2c.b_macid_cfg.ramask1 = (ramask >> 8) & 0xff;
	h2c.b_macid_cfg.ramask2 = (ramask >> 16) & 0xff;
	h2c.b_macid_cfg.ramask3 = (ramask >> 24) & 0xff;

	h2c.ramask.arg = 0x80;
	h2c.b_macid_cfg.data1 = 0;
	if (sgi)
		h2c.b_macid_cfg.data1 |= BIT(7);

	h2c.b_macid_cfg.data2 = bw;

	dev_dbg(&priv->udev->dev, "%s: rate mask %08x, arg %02x, size %zi\n",
		__func__, ramask, h2c.ramask.arg, sizeof(h2c.b_macid_cfg));
	rtl8xxxu_gen2_h2c_cmd(priv, &h2c, sizeof(h2c.b_macid_cfg));
}