static int isdn_header_cache(const struct neighbour *neigh, struct hh_cache *hh,
			     __be16 type)
{
	const struct net_device *dev = neigh->dev;
	isdn_net_local *lp = netdev_priv(dev);

	if (lp->p_encap == ISDN_NET_ENCAP_ETHER)
		return eth_header_cache(neigh, hh, type);
	return -1;
}