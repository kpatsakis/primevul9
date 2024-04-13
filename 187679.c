static int yam_seq_show(struct seq_file *seq, void *v)
{
	struct net_device *dev = v;
	const struct yam_port *yp = netdev_priv(dev);

	seq_printf(seq, "Device %s\n", dev->name);
	seq_printf(seq, "  Up       %d\n", netif_running(dev));
	seq_printf(seq, "  Speed    %u\n", yp->bitrate);
	seq_printf(seq, "  IoBase   0x%x\n", yp->iobase);
	seq_printf(seq, "  BaudRate %u\n", yp->baudrate);
	seq_printf(seq, "  IRQ      %u\n", yp->irq);
	seq_printf(seq, "  TxState  %u\n", yp->tx_state);
	seq_printf(seq, "  Duplex   %u\n", yp->dupmode);
	seq_printf(seq, "  HoldDly  %u\n", yp->holdd);
	seq_printf(seq, "  TxDelay  %u\n", yp->txd);
	seq_printf(seq, "  TxTail   %u\n", yp->txtail);
	seq_printf(seq, "  SlotTime %u\n", yp->slot);
	seq_printf(seq, "  Persist  %u\n", yp->pers);
	seq_printf(seq, "  TxFrames %lu\n", dev->stats.tx_packets);
	seq_printf(seq, "  RxFrames %lu\n", dev->stats.rx_packets);
	seq_printf(seq, "  TxInt    %u\n", yp->nb_mdint);
	seq_printf(seq, "  RxInt    %u\n", yp->nb_rxint);
	seq_printf(seq, "  RxOver   %lu\n", dev->stats.rx_fifo_errors);
	seq_printf(seq, "\n");
	return 0;
}