static int ipv6_generate_eui64(u8 *eui, struct net_device *dev)
{
	switch (dev->type) {
	case ARPHRD_ETHER:
	case ARPHRD_FDDI:
		return addrconf_ifid_eui48(eui, dev);
	case ARPHRD_ARCNET:
		return addrconf_ifid_arcnet(eui, dev);
	case ARPHRD_INFINIBAND:
		return addrconf_ifid_infiniband(eui, dev);
	case ARPHRD_SIT:
		return addrconf_ifid_sit(eui, dev);
	case ARPHRD_IPGRE:
		return addrconf_ifid_gre(eui, dev);
	case ARPHRD_IEEE802154:
		return addrconf_ifid_eui64(eui, dev);
	case ARPHRD_IEEE1394:
		return addrconf_ifid_ieee1394(eui, dev);
	}
	return -1;
}