temac_poll_controller(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);

	disable_irq(lp->tx_irq);
	disable_irq(lp->rx_irq);

	ll_temac_rx_irq(lp->tx_irq, ndev);
	ll_temac_tx_irq(lp->rx_irq, ndev);

	enable_irq(lp->tx_irq);
	enable_irq(lp->rx_irq);
}