static void temac_dma_bd_release(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	int i;

	/* Reset Local Link (DMA) */
	lp->dma_out(lp, DMA_CONTROL_REG, DMA_CONTROL_RST);

	for (i = 0; i < lp->rx_bd_num; i++) {
		if (!lp->rx_skb[i])
			break;
		else {
			dma_unmap_single(ndev->dev.parent, lp->rx_bd_v[i].phys,
					XTE_MAX_JUMBO_FRAME_SIZE, DMA_FROM_DEVICE);
			dev_kfree_skb(lp->rx_skb[i]);
		}
	}
	if (lp->rx_bd_v)
		dma_free_coherent(ndev->dev.parent,
				  sizeof(*lp->rx_bd_v) * lp->rx_bd_num,
				  lp->rx_bd_v, lp->rx_bd_p);
	if (lp->tx_bd_v)
		dma_free_coherent(ndev->dev.parent,
				  sizeof(*lp->tx_bd_v) * lp->tx_bd_num,
				  lp->tx_bd_v, lp->tx_bd_p);
}