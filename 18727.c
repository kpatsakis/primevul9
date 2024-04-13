static irqreturn_t xemaclite_interrupt(int irq, void *dev_id)
{
	bool tx_complete = false;
	struct net_device *dev = dev_id;
	struct net_local *lp = netdev_priv(dev);
	void __iomem *base_addr = lp->base_addr;
	u32 tx_status;

	/* Check if there is Rx Data available */
	if ((xemaclite_readl(base_addr + XEL_RSR_OFFSET) &
			 XEL_RSR_RECV_DONE_MASK) ||
	    (xemaclite_readl(base_addr + XEL_BUFFER_OFFSET + XEL_RSR_OFFSET)
			 & XEL_RSR_RECV_DONE_MASK))

		xemaclite_rx_handler(dev);

	/* Check if the Transmission for the first buffer is completed */
	tx_status = xemaclite_readl(base_addr + XEL_TSR_OFFSET);
	if (((tx_status & XEL_TSR_XMIT_BUSY_MASK) == 0) &&
		(tx_status & XEL_TSR_XMIT_ACTIVE_MASK) != 0) {

		tx_status &= ~XEL_TSR_XMIT_ACTIVE_MASK;
		xemaclite_writel(tx_status, base_addr + XEL_TSR_OFFSET);

		tx_complete = true;
	}

	/* Check if the Transmission for the second buffer is completed */
	tx_status = xemaclite_readl(base_addr + XEL_BUFFER_OFFSET + XEL_TSR_OFFSET);
	if (((tx_status & XEL_TSR_XMIT_BUSY_MASK) == 0) &&
		(tx_status & XEL_TSR_XMIT_ACTIVE_MASK) != 0) {

		tx_status &= ~XEL_TSR_XMIT_ACTIVE_MASK;
		xemaclite_writel(tx_status, base_addr + XEL_BUFFER_OFFSET +
				 XEL_TSR_OFFSET);

		tx_complete = true;
	}

	/* If there was a Tx interrupt, call the Tx Handler */
	if (tx_complete != 0)
		xemaclite_tx_handler(dev);

	return IRQ_HANDLED;
}