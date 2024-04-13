void bond_setup(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);

	spin_lock_init(&bond->mode_lock);
	bond->params = bonding_defaults;

	/* Initialize pointers */
	bond->dev = bond_dev;

	/* Initialize the device entry points */
	ether_setup(bond_dev);
	bond_dev->max_mtu = ETH_MAX_MTU;
	bond_dev->netdev_ops = &bond_netdev_ops;
	bond_dev->ethtool_ops = &bond_ethtool_ops;

	bond_dev->needs_free_netdev = true;
	bond_dev->priv_destructor = bond_destructor;

	SET_NETDEV_DEVTYPE(bond_dev, &bond_type);

	/* Initialize the device options */
	bond_dev->flags |= IFF_MASTER;
	bond_dev->priv_flags |= IFF_BONDING | IFF_UNICAST_FLT | IFF_NO_QUEUE;
	bond_dev->priv_flags &= ~(IFF_XMIT_DST_RELEASE | IFF_TX_SKB_SHARING);

#ifdef CONFIG_XFRM_OFFLOAD
	/* set up xfrm device ops (only supported in active-backup right now) */
	bond_dev->xfrmdev_ops = &bond_xfrmdev_ops;
	bond->xs = NULL;
#endif /* CONFIG_XFRM_OFFLOAD */

	/* don't acquire bond device's netif_tx_lock when transmitting */
	bond_dev->features |= NETIF_F_LLTX;

	/* By default, we declare the bond to be fully
	 * VLAN hardware accelerated capable. Special
	 * care is taken in the various xmit functions
	 * when there are slaves that are not hw accel
	 * capable
	 */

	/* Don't allow bond devices to change network namespaces. */
	bond_dev->features |= NETIF_F_NETNS_LOCAL;

	bond_dev->hw_features = BOND_VLAN_FEATURES |
				NETIF_F_HW_VLAN_CTAG_RX |
				NETIF_F_HW_VLAN_CTAG_FILTER;

	bond_dev->hw_features |= NETIF_F_GSO_ENCAP_ALL;
	bond_dev->features |= bond_dev->hw_features;
	bond_dev->features |= NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_STAG_TX;
#ifdef CONFIG_XFRM_OFFLOAD
	bond_dev->hw_features |= BOND_XFRM_FEATURES;
	/* Only enable XFRM features if this is an active-backup config */
	if (BOND_MODE(bond) == BOND_MODE_ACTIVEBACKUP)
		bond_dev->features |= BOND_XFRM_FEATURES;
#endif /* CONFIG_XFRM_OFFLOAD */
#if IS_ENABLED(CONFIG_TLS_DEVICE)
	if (bond_sk_check(bond))
		bond_dev->features |= BOND_TLS_FEATURES;
#endif
}