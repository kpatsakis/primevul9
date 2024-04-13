void rtl8xxxu_gen2_usb_quirks(struct rtl8xxxu_priv *priv)
{
	u32 val32;

	val32 = rtl8xxxu_read32(priv, REG_TXDMA_OFFSET_CHK);
	val32 |= TXDMA_OFFSET_DROP_DATA_EN;
	rtl8xxxu_write32(priv, REG_TXDMA_OFFSET_CHK, val32);
}