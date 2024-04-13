static int bnx2x_validate_addr(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);

	/* query the bulletin board for mac address configured by the PF */
	if (IS_VF(bp))
		bnx2x_sample_bulletin(bp);

	if (!is_valid_ether_addr(dev->dev_addr)) {
		BNX2X_ERR("Non-valid Ethernet address\n");
		return -EADDRNOTAVAIL;
	}
	return 0;
}