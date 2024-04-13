static void isdn_header_cache_update(struct hh_cache *hh,
				     const struct net_device *dev,
				     const unsigned char *haddr)
{
	isdn_net_local *lp = netdev_priv(dev);
	if (lp->p_encap == ISDN_NET_ENCAP_ETHER)
		eth_header_cache_update(hh, dev, haddr);
}