static void rtl8xxxu_stop(struct ieee80211_hw *hw)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	unsigned long flags;

	rtl8xxxu_write8(priv, REG_TXPAUSE, 0xff);

	rtl8xxxu_write16(priv, REG_RXFLTMAP0, 0x0000);
	rtl8xxxu_write16(priv, REG_RXFLTMAP2, 0x0000);

	spin_lock_irqsave(&priv->rx_urb_lock, flags);
	priv->shutdown = true;
	spin_unlock_irqrestore(&priv->rx_urb_lock, flags);

	usb_kill_anchored_urbs(&priv->rx_anchor);
	usb_kill_anchored_urbs(&priv->tx_anchor);
	if (priv->usb_interrupts)
		usb_kill_anchored_urbs(&priv->int_anchor);

	rtl8xxxu_write8(priv, REG_TXPAUSE, 0xff);

	priv->fops->disable_rf(priv);

	/*
	 * Disable interrupts
	 */
	if (priv->usb_interrupts)
		rtl8xxxu_write32(priv, REG_USB_HIMR, 0);

	rtl8xxxu_free_rx_resources(priv);
	rtl8xxxu_free_tx_resources(priv);
}