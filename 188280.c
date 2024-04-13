static int temac_dma_bd_init(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct sk_buff *skb;
	dma_addr_t skb_dma_addr;
	int i;

	lp->rx_skb = devm_kcalloc(&ndev->dev, lp->rx_bd_num,
				  sizeof(*lp->rx_skb), GFP_KERNEL);
	if (!lp->rx_skb)
		goto out;

	/* allocate the tx and rx ring buffer descriptors. */
	/* returns a virtual address and a physical address. */
	lp->tx_bd_v = dma_alloc_coherent(ndev->dev.parent,
					 sizeof(*lp->tx_bd_v) * lp->tx_bd_num,
					 &lp->tx_bd_p, GFP_KERNEL);
	if (!lp->tx_bd_v)
		goto out;

	lp->rx_bd_v = dma_alloc_coherent(ndev->dev.parent,
					 sizeof(*lp->rx_bd_v) * lp->rx_bd_num,
					 &lp->rx_bd_p, GFP_KERNEL);
	if (!lp->rx_bd_v)
		goto out;

	for (i = 0; i < lp->tx_bd_num; i++) {
		lp->tx_bd_v[i].next = cpu_to_be32(lp->tx_bd_p
			+ sizeof(*lp->tx_bd_v) * ((i + 1) % lp->tx_bd_num));
	}

	for (i = 0; i < lp->rx_bd_num; i++) {
		lp->rx_bd_v[i].next = cpu_to_be32(lp->rx_bd_p
			+ sizeof(*lp->rx_bd_v) * ((i + 1) % lp->rx_bd_num));

		skb = netdev_alloc_skb_ip_align(ndev,
						XTE_MAX_JUMBO_FRAME_SIZE);
		if (!skb)
			goto out;

		lp->rx_skb[i] = skb;
		/* returns physical address of skb->data */
		skb_dma_addr = dma_map_single(ndev->dev.parent, skb->data,
					      XTE_MAX_JUMBO_FRAME_SIZE,
					      DMA_FROM_DEVICE);
		if (dma_mapping_error(ndev->dev.parent, skb_dma_addr))
			goto out;
		lp->rx_bd_v[i].phys = cpu_to_be32(skb_dma_addr);
		lp->rx_bd_v[i].len = cpu_to_be32(XTE_MAX_JUMBO_FRAME_SIZE);
		lp->rx_bd_v[i].app0 = cpu_to_be32(STS_CTRL_APP0_IRQONEND);
	}

	/* Configure DMA channel (irq setup) */
	lp->dma_out(lp, TX_CHNL_CTRL,
		    lp->coalesce_delay_tx << 24 | lp->coalesce_count_tx << 16 |
		    0x00000400 | // Use 1 Bit Wide Counters. Currently Not Used!
		    CHNL_CTRL_IRQ_EN | CHNL_CTRL_IRQ_ERR_EN |
		    CHNL_CTRL_IRQ_DLY_EN | CHNL_CTRL_IRQ_COAL_EN);
	lp->dma_out(lp, RX_CHNL_CTRL,
		    lp->coalesce_delay_rx << 24 | lp->coalesce_count_rx << 16 |
		    CHNL_CTRL_IRQ_IOE |
		    CHNL_CTRL_IRQ_EN | CHNL_CTRL_IRQ_ERR_EN |
		    CHNL_CTRL_IRQ_DLY_EN | CHNL_CTRL_IRQ_COAL_EN);

	/* Init descriptor indexes */
	lp->tx_bd_ci = 0;
	lp->tx_bd_tail = 0;
	lp->rx_bd_ci = 0;
	lp->rx_bd_tail = lp->rx_bd_num - 1;

	/* Enable RX DMA transfers */
	wmb();
	lp->dma_out(lp, RX_CURDESC_PTR,  lp->rx_bd_p);
	lp->dma_out(lp, RX_TAILDESC_PTR,
		       lp->rx_bd_p + (sizeof(*lp->rx_bd_v) * lp->rx_bd_tail));

	/* Prepare for TX DMA transfer */
	lp->dma_out(lp, TX_CURDESC_PTR, lp->tx_bd_p);

	return 0;

out:
	temac_dma_bd_release(ndev);
	return -ENOMEM;
}