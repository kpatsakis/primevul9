static int rtl8xxxu_init_queue_priority(struct rtl8xxxu_priv *priv)
{
	u16 val16, hi, lo;
	u16 hiq, mgq, bkq, beq, viq, voq;
	int hip, mgp, bkp, bep, vip, vop;
	int ret = 0;

	switch (priv->ep_tx_count) {
	case 1:
		if (priv->ep_tx_high_queue) {
			hi = TRXDMA_QUEUE_HIGH;
		} else if (priv->ep_tx_low_queue) {
			hi = TRXDMA_QUEUE_LOW;
		} else if (priv->ep_tx_normal_queue) {
			hi = TRXDMA_QUEUE_NORMAL;
		} else {
			hi = 0;
			ret = -EINVAL;
		}

		hiq = hi;
		mgq = hi;
		bkq = hi;
		beq = hi;
		viq = hi;
		voq = hi;

		hip = 0;
		mgp = 0;
		bkp = 0;
		bep = 0;
		vip = 0;
		vop = 0;
		break;
	case 2:
		if (priv->ep_tx_high_queue && priv->ep_tx_low_queue) {
			hi = TRXDMA_QUEUE_HIGH;
			lo = TRXDMA_QUEUE_LOW;
		} else if (priv->ep_tx_normal_queue && priv->ep_tx_low_queue) {
			hi = TRXDMA_QUEUE_NORMAL;
			lo = TRXDMA_QUEUE_LOW;
		} else if (priv->ep_tx_high_queue && priv->ep_tx_normal_queue) {
			hi = TRXDMA_QUEUE_HIGH;
			lo = TRXDMA_QUEUE_NORMAL;
		} else {
			ret = -EINVAL;
			hi = 0;
			lo = 0;
		}

		hiq = hi;
		mgq = hi;
		bkq = lo;
		beq = lo;
		viq = hi;
		voq = hi;

		hip = 0;
		mgp = 0;
		bkp = 1;
		bep = 1;
		vip = 0;
		vop = 0;
		break;
	case 3:
		beq = TRXDMA_QUEUE_LOW;
		bkq = TRXDMA_QUEUE_LOW;
		viq = TRXDMA_QUEUE_NORMAL;
		voq = TRXDMA_QUEUE_HIGH;
		mgq = TRXDMA_QUEUE_HIGH;
		hiq = TRXDMA_QUEUE_HIGH;

		hip = hiq ^ 3;
		mgp = mgq ^ 3;
		bkp = bkq ^ 3;
		bep = beq ^ 3;
		vip = viq ^ 3;
		vop = viq ^ 3;
		break;
	default:
		ret = -EINVAL;
	}

	/*
	 * None of the vendor drivers are configuring the beacon
	 * queue here .... why?
	 */
	if (!ret) {
		val16 = rtl8xxxu_read16(priv, REG_TRXDMA_CTRL);
		val16 &= 0x7;
		val16 |= (voq << TRXDMA_CTRL_VOQ_SHIFT) |
			(viq << TRXDMA_CTRL_VIQ_SHIFT) |
			(beq << TRXDMA_CTRL_BEQ_SHIFT) |
			(bkq << TRXDMA_CTRL_BKQ_SHIFT) |
			(mgq << TRXDMA_CTRL_MGQ_SHIFT) |
			(hiq << TRXDMA_CTRL_HIQ_SHIFT);
		rtl8xxxu_write16(priv, REG_TRXDMA_CTRL, val16);

		priv->pipe_out[TXDESC_QUEUE_VO] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[vop]);
		priv->pipe_out[TXDESC_QUEUE_VI] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[vip]);
		priv->pipe_out[TXDESC_QUEUE_BE] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[bep]);
		priv->pipe_out[TXDESC_QUEUE_BK] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[bkp]);
		priv->pipe_out[TXDESC_QUEUE_BEACON] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[0]);
		priv->pipe_out[TXDESC_QUEUE_MGNT] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[mgp]);
		priv->pipe_out[TXDESC_QUEUE_HIGH] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[hip]);
		priv->pipe_out[TXDESC_QUEUE_CMD] =
			usb_sndbulkpipe(priv->udev, priv->out_ep[0]);
	}

	return ret;
}