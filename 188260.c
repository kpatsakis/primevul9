static void temac_start_xmit_done(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct cdmac_bd *cur_p;
	unsigned int stat = 0;
	struct sk_buff *skb;

	cur_p = &lp->tx_bd_v[lp->tx_bd_ci];
	stat = be32_to_cpu(cur_p->app0);

	while (stat & STS_CTRL_APP0_CMPLT) {
		/* Make sure that the other fields are read after bd is
		 * released by dma
		 */
		rmb();
		dma_unmap_single(ndev->dev.parent, be32_to_cpu(cur_p->phys),
				 be32_to_cpu(cur_p->len), DMA_TO_DEVICE);
		skb = (struct sk_buff *)ptr_from_txbd(cur_p);
		if (skb)
			dev_consume_skb_irq(skb);
		cur_p->app1 = 0;
		cur_p->app2 = 0;
		cur_p->app3 = 0;
		cur_p->app4 = 0;

		ndev->stats.tx_packets++;
		ndev->stats.tx_bytes += be32_to_cpu(cur_p->len);

		/* app0 must be visible last, as it is used to flag
		 * availability of the bd
		 */
		smp_mb();
		cur_p->app0 = 0;

		lp->tx_bd_ci++;
		if (lp->tx_bd_ci >= lp->tx_bd_num)
			lp->tx_bd_ci = 0;

		cur_p = &lp->tx_bd_v[lp->tx_bd_ci];
		stat = be32_to_cpu(cur_p->app0);
	}

	/* Matches barrier in temac_start_xmit */
	smp_mb();

	netif_wake_queue(ndev);
}