void rtl8xxxu_gen1_usb_quirks(struct rtl8xxxu_priv *priv)
{
	/* Fix USB interface interference issue */
	rtl8xxxu_write8(priv, 0xfe40, 0xe0);
	rtl8xxxu_write8(priv, 0xfe41, 0x8d);
	rtl8xxxu_write8(priv, 0xfe42, 0x80);
	/*
	 * This sets TXDMA_OFFSET_DROP_DATA_EN (bit 9) as well as bits
	 * 8 and 5, for which I have found no documentation.
	 */
	rtl8xxxu_write32(priv, REG_TXDMA_OFFSET_CHK, 0xfd0320);

	/*
	 * Solve too many protocol error on USB bus.
	 * Can't do this for 8188/8192 UMC A cut parts
	 */
	if (!(!priv->chip_cut && priv->vendor_umc)) {
		rtl8xxxu_write8(priv, 0xfe40, 0xe6);
		rtl8xxxu_write8(priv, 0xfe41, 0x94);
		rtl8xxxu_write8(priv, 0xfe42, 0x80);

		rtl8xxxu_write8(priv, 0xfe40, 0xe0);
		rtl8xxxu_write8(priv, 0xfe41, 0x19);
		rtl8xxxu_write8(priv, 0xfe42, 0x80);

		rtl8xxxu_write8(priv, 0xfe40, 0xe5);
		rtl8xxxu_write8(priv, 0xfe41, 0x91);
		rtl8xxxu_write8(priv, 0xfe42, 0x80);

		rtl8xxxu_write8(priv, 0xfe40, 0xe2);
		rtl8xxxu_write8(priv, 0xfe41, 0x81);
		rtl8xxxu_write8(priv, 0xfe42, 0x80);
	}
}