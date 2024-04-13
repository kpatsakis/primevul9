static int ll_temac_ethtools_get_coalesce(struct net_device *ndev,
					  struct ethtool_coalesce *ec)
{
	struct temac_local *lp = netdev_priv(ndev);

	ec->rx_max_coalesced_frames = lp->coalesce_count_rx;
	ec->tx_max_coalesced_frames = lp->coalesce_count_tx;
	ec->rx_coalesce_usecs = (lp->coalesce_delay_rx * 512) / 100;
	ec->tx_coalesce_usecs = (lp->coalesce_delay_tx * 512) / 100;
	return 0;
}