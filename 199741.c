static int addrconf_ifid_eui48(u8 *eui, struct net_device *dev)
{
	if (dev->addr_len != ETH_ALEN)
		return -1;
	memcpy(eui, dev->dev_addr, 3);
	memcpy(eui + 5, dev->dev_addr + 3, 3);

	/*
	 * The zSeries OSA network cards can be shared among various
	 * OS instances, but the OSA cards have only one MAC address.
	 * This leads to duplicate address conflicts in conjunction
	 * with IPv6 if more than one instance uses the same card.
	 *
	 * The driver for these cards can deliver a unique 16-bit
	 * identifier for each instance sharing the same card.  It is
	 * placed instead of 0xFFFE in the interface identifier.  The
	 * "u" bit of the interface identifier is not inverted in this
	 * case.  Hence the resulting interface identifier has local
	 * scope according to RFC2373.
	 */
	if (dev->dev_id) {
		eui[3] = (dev->dev_id >> 8) & 0xFF;
		eui[4] = dev->dev_id & 0xFF;
	} else {
		eui[3] = 0xFF;
		eui[4] = 0xFE;
		eui[0] ^= 2;
	}
	return 0;
}