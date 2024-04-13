static void yam_setup(struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);

	yp->magic = YAM_MAGIC;
	yp->bitrate = DEFAULT_BITRATE;
	yp->baudrate = DEFAULT_BITRATE * 2;
	yp->iobase = 0;
	yp->irq = 0;
	yp->dupmode = 0;
	yp->holdd = DEFAULT_HOLDD;
	yp->txd = DEFAULT_TXD;
	yp->txtail = DEFAULT_TXTAIL;
	yp->slot = DEFAULT_SLOT;
	yp->pers = DEFAULT_PERS;
	yp->dev = dev;

	dev->base_addr = yp->iobase;
	dev->irq = yp->irq;

	skb_queue_head_init(&yp->send_queue);

	dev->netdev_ops = &yam_netdev_ops;
	dev->header_ops = &ax25_header_ops;

	dev->type = ARPHRD_AX25;
	dev->hard_header_len = AX25_MAX_HEADER_LEN;
	dev->mtu = AX25_MTU;
	dev->addr_len = AX25_ADDR_LEN;
	memcpy(dev->broadcast, &ax25_bcast, AX25_ADDR_LEN);
	dev_addr_set(dev, (u8 *)&ax25_defaddr);
}