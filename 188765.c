int i40e_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_pf *pf = np->vsi->back;

	switch (cmd) {
	case SIOCGHWTSTAMP:
		return i40e_ptp_get_ts_config(pf, ifr);
	case SIOCSHWTSTAMP:
		return i40e_ptp_set_ts_config(pf, ifr);
	default:
		return -EOPNOTSUPP;
	}
}