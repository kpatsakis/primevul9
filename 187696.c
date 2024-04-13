static int yam_close(struct net_device *dev)
{
	struct sk_buff *skb;
	struct yam_port *yp = netdev_priv(dev);

	if (!dev)
		return -EINVAL;

	/*
	 * disable interrupts
	 */
	outb(0, IER(dev->base_addr));
	outb(1, MCR(dev->base_addr));
	/* Remove IRQ handler if last */
	free_irq(dev->irq,dev);
	release_region(dev->base_addr, YAM_EXTENT);
	netif_stop_queue(dev);
	while ((skb = skb_dequeue(&yp->send_queue)))
		dev_kfree_skb(skb);

	printk(KERN_INFO "%s: close yam at iobase 0x%lx irq %u\n",
		   yam_drvname, dev->base_addr, dev->irq);
	return 0;
}