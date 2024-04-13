static int temac_stop(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct phy_device *phydev = ndev->phydev;

	dev_dbg(&ndev->dev, "temac_close()\n");

	cancel_delayed_work_sync(&lp->restart_work);

	free_irq(lp->tx_irq, ndev);
	free_irq(lp->rx_irq, ndev);

	if (phydev)
		phy_disconnect(phydev);

	temac_dma_bd_release(ndev);

	return 0;
}