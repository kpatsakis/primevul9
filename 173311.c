static int bnx2x_close(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);

	/* Unload the driver, release IRQs */
	bnx2x_nic_unload(bp, UNLOAD_CLOSE, false);

	return 0;
}