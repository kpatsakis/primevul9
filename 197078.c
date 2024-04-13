static netdev_features_t bond_fix_features(struct net_device *dev,
					   netdev_features_t features)
{
	struct bonding *bond = netdev_priv(dev);
	struct list_head *iter;
	netdev_features_t mask;
	struct slave *slave;

#if IS_ENABLED(CONFIG_TLS_DEVICE)
	if (bond_sk_check(bond))
		features |= BOND_TLS_FEATURES;
	else
		features &= ~BOND_TLS_FEATURES;
#endif

	mask = features;

	features &= ~NETIF_F_ONE_FOR_ALL;
	features |= NETIF_F_ALL_FOR_ALL;

	bond_for_each_slave(bond, slave, iter) {
		features = netdev_increment_features(features,
						     slave->dev->features,
						     mask);
	}
	features = netdev_add_tso_features(features, mask);

	return features;
}