isdn_net_init(struct net_device *ndev)
{
	ushort max_hlhdr_len = 0;
	int drvidx;

	/*
	 *  up till binding we ask the protocol layer to reserve as much
	 *  as we might need for HL layer
	 */

	for (drvidx = 0; drvidx < ISDN_MAX_DRIVERS; drvidx++)
		if (dev->drv[drvidx])
			if (max_hlhdr_len < dev->drv[drvidx]->interface->hl_hdrlen)
				max_hlhdr_len = dev->drv[drvidx]->interface->hl_hdrlen;

	ndev->hard_header_len = ETH_HLEN + max_hlhdr_len;
	return 0;
}