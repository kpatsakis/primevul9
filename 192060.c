void rtl8xxxu_gen1_init_aggregation(struct rtl8xxxu_priv *priv)
{
	u8 agg_ctrl, usb_spec, page_thresh, timeout;

	usb_spec = rtl8xxxu_read8(priv, REG_USB_SPECIAL_OPTION);
	usb_spec &= ~USB_SPEC_USB_AGG_ENABLE;
	rtl8xxxu_write8(priv, REG_USB_SPECIAL_OPTION, usb_spec);

	agg_ctrl = rtl8xxxu_read8(priv, REG_TRXDMA_CTRL);
	agg_ctrl &= ~TRXDMA_CTRL_RXDMA_AGG_EN;

	if (!rtl8xxxu_dma_aggregation) {
		rtl8xxxu_write8(priv, REG_TRXDMA_CTRL, agg_ctrl);
		return;
	}

	agg_ctrl |= TRXDMA_CTRL_RXDMA_AGG_EN;
	rtl8xxxu_write8(priv, REG_TRXDMA_CTRL, agg_ctrl);

	/*
	 * The number of packets we can take looks to be buffer size / 512
	 * which matches the 512 byte rounding we have to do when de-muxing
	 * the packets.
	 *
	 * Sample numbers from the vendor driver:
	 * USB High-Speed mode values:
	 *   RxAggBlockCount = 8 : 512 byte unit
	 *   RxAggBlockTimeout = 6
	 *   RxAggPageCount = 48 : 128 byte unit
	 *   RxAggPageTimeout = 4 or 6 (absolute time 34ms/(2^6))
	 */

	page_thresh = (priv->fops->rx_agg_buf_size / 512);
	if (rtl8xxxu_dma_agg_pages >= 0) {
		if (rtl8xxxu_dma_agg_pages <= page_thresh)
			timeout = page_thresh;
		else if (rtl8xxxu_dma_agg_pages <= 6)
			dev_err(&priv->udev->dev,
				"%s: dma_agg_pages=%i too small, minimum is 6\n",
				__func__, rtl8xxxu_dma_agg_pages);
		else
			dev_err(&priv->udev->dev,
				"%s: dma_agg_pages=%i larger than limit %i\n",
				__func__, rtl8xxxu_dma_agg_pages, page_thresh);
	}
	rtl8xxxu_write8(priv, REG_RXDMA_AGG_PG_TH, page_thresh);
	/*
	 * REG_RXDMA_AGG_PG_TH + 1 seems to be the timeout register on
	 * gen2 chips and rtl8188eu. The rtl8723au seems unhappy if we
	 * don't set it, so better set both.
	 */
	timeout = 4;

	if (rtl8xxxu_dma_agg_timeout >= 0) {
		if (rtl8xxxu_dma_agg_timeout <= 127)
			timeout = rtl8xxxu_dma_agg_timeout;
		else
			dev_err(&priv->udev->dev,
				"%s: Invalid dma_agg_timeout: %i\n",
				__func__, rtl8xxxu_dma_agg_timeout);
	}

	rtl8xxxu_write8(priv, REG_RXDMA_AGG_PG_TH + 1, timeout);
	rtl8xxxu_write8(priv, REG_USB_DMA_AGG_TO, timeout);
	priv->rx_buf_aggregation = 1;
}