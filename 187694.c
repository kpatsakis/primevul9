static void yam_set_uart(struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);
	int divisor = 115200 / yp->baudrate;

	outb(0, IER(dev->base_addr));
	outb(LCR_DLAB | LCR_BIT8, LCR(dev->base_addr));
	outb(divisor, DLL(dev->base_addr));
	outb(0, DLM(dev->base_addr));
	outb(LCR_BIT8, LCR(dev->base_addr));
	outb(PTT_OFF, MCR(dev->base_addr));
	outb(0x00, FCR(dev->base_addr));

	/* Flush pending irq */

	inb(RBR(dev->base_addr));
	inb(MSR(dev->base_addr));

	/* Enable rx irq */

	outb(ENABLE_RTXINT, IER(dev->base_addr));
}