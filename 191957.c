int rtl8xxxu_flush_fifo(struct rtl8xxxu_priv *priv)
{
	struct device *dev = &priv->udev->dev;
	u32 val32;
	int retry, retval;

	rtl8xxxu_write8(priv, REG_TXPAUSE, 0xff);

	val32 = rtl8xxxu_read32(priv, REG_RXPKT_NUM);
	val32 |= RXPKT_NUM_RW_RELEASE_EN;
	rtl8xxxu_write32(priv, REG_RXPKT_NUM, val32);

	retry = 100;
	retval = -EBUSY;

	do {
		val32 = rtl8xxxu_read32(priv, REG_RXPKT_NUM);
		if (val32 & RXPKT_NUM_RXDMA_IDLE) {
			retval = 0;
			break;
		}
	} while (retry--);

	rtl8xxxu_write16(priv, REG_RQPN_NPQ, 0);
	rtl8xxxu_write32(priv, REG_RQPN, 0x80000000);
	mdelay(2);

	if (!retry)
		dev_warn(dev, "Failed to flush FIFO\n");

	return retval;
}