static int ll_temac_ethtools_set_coalesce(struct net_device *ndev,
					  struct ethtool_coalesce *ec)
{
	struct temac_local *lp = netdev_priv(ndev);

	if (netif_running(ndev)) {
		netdev_err(ndev,
			   "Please stop netif before applying configuration\n");
		return -EFAULT;
	}

	if (ec->rx_max_coalesced_frames)
		lp->coalesce_count_rx = ec->rx_max_coalesced_frames;
	if (ec->tx_max_coalesced_frames)
		lp->coalesce_count_tx = ec->tx_max_coalesced_frames;
	/* With typical LocalLink clock speed of 200 MHz and
	 * C_PRESCALAR=1023, each delay count corresponds to 5.12 us.
	 */
	if (ec->rx_coalesce_usecs)
		lp->coalesce_delay_rx =
			min(255U, (ec->rx_coalesce_usecs * 100) / 512);
	if (ec->tx_coalesce_usecs)
		lp->coalesce_delay_tx =
			min(255U, (ec->tx_coalesce_usecs * 100) / 512);

	return 0;
}