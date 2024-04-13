static irqreturn_t yam_interrupt(int irq, void *dev_id)
{
	struct net_device *dev;
	struct yam_port *yp;
	unsigned char iir;
	int counter = 100;
	int i;
	int handled = 0;

	for (i = 0; i < NR_PORTS; i++) {
		dev = yam_devs[i];
		yp = netdev_priv(dev);

		if (!netif_running(dev))
			continue;

		while ((iir = IIR_MASK & inb(IIR(dev->base_addr))) != IIR_NOPEND) {
			unsigned char msr = inb(MSR(dev->base_addr));
			unsigned char lsr = inb(LSR(dev->base_addr));
			unsigned char rxb;

			handled = 1;

			if (lsr & LSR_OE)
				++dev->stats.rx_fifo_errors;

			yp->dcd = (msr & RX_DCD) ? 1 : 0;

			if (--counter <= 0) {
				printk(KERN_ERR "%s: too many irq iir=%d\n",
						dev->name, iir);
				goto out;
			}
			if (msr & TX_RDY) {
				++yp->nb_mdint;
				yam_tx_byte(dev, yp);
			}
			if (lsr & LSR_RXC) {
				++yp->nb_rxint;
				rxb = inb(RBR(dev->base_addr));
				if (msr & RX_FLAG)
					yam_rx_flag(dev, yp);
				else
					yam_rx_byte(dev, yp, rxb);
			}
		}
	}
out:
	return IRQ_RETVAL(handled);
}