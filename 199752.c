static int addrconf_ifid_ieee1394(u8 *eui, struct net_device *dev)
{
	union fwnet_hwaddr *ha;

	if (dev->addr_len != FWNET_ALEN)
		return -1;

	ha = (union fwnet_hwaddr *)dev->dev_addr;

	memcpy(eui, &ha->uc.uniq_id, sizeof(ha->uc.uniq_id));
	eui[0] ^= 2;
	return 0;
}