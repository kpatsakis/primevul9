static int yam_open(struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);
	enum uart u;
	int i;
	int ret=0;

	printk(KERN_INFO "Trying %s at iobase 0x%lx irq %u\n", dev->name, dev->base_addr, dev->irq);

	if (!yp->bitrate)
		return -ENXIO;
	if (!dev->base_addr || dev->base_addr > 0x1000 - YAM_EXTENT ||
		dev->irq < 2 || dev->irq > 15) {
		return -ENXIO;
	}
	if (!request_region(dev->base_addr, YAM_EXTENT, dev->name))
	{
		printk(KERN_ERR "%s: cannot 0x%lx busy\n", dev->name, dev->base_addr);
		return -EACCES;
	}
	if ((u = yam_check_uart(dev->base_addr)) == c_uart_unknown) {
		printk(KERN_ERR "%s: cannot find uart type\n", dev->name);
		ret = -EIO;
		goto out_release_base;
	}
	if (fpga_download(dev->base_addr, yp->bitrate)) {
		printk(KERN_ERR "%s: cannot init FPGA\n", dev->name);
		ret = -EIO;
		goto out_release_base;
	}
	outb(0, IER(dev->base_addr));
	if (request_irq(dev->irq, yam_interrupt, IRQF_SHARED, dev->name, dev)) {
		printk(KERN_ERR "%s: irq %d busy\n", dev->name, dev->irq);
		ret = -EBUSY;
		goto out_release_base;
	}

	yam_set_uart(dev);

	netif_start_queue(dev);
	
	yp->slotcnt = yp->slot / 10;

	/* Reset overruns for all ports - FPGA programming makes overruns */
	for (i = 0; i < NR_PORTS; i++) {
		struct net_device *yam_dev = yam_devs[i];

		inb(LSR(yam_dev->base_addr));
		yam_dev->stats.rx_fifo_errors = 0;
	}

	printk(KERN_INFO "%s at iobase 0x%lx irq %u uart %s\n", dev->name, dev->base_addr, dev->irq,
		   uart_str[u]);
	return 0;

out_release_base:
	release_region(dev->base_addr, YAM_EXTENT);
	return ret;
}