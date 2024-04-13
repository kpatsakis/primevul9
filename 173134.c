static int bnx2x_mdio_write(struct net_device *netdev, int prtad, int devad,
			    u16 addr, u16 value)
{
	struct bnx2x *bp = netdev_priv(netdev);
	int rc;

	DP(NETIF_MSG_LINK,
	   "mdio_write: prtad 0x%x, devad 0x%x, addr 0x%x, value 0x%x\n",
	   prtad, devad, addr, value);

	/* The HW expects different devad if CL22 is used */
	devad = (devad == MDIO_DEVAD_NONE) ? DEFAULT_PHY_DEV_ADDR : devad;

	bnx2x_acquire_phy_lock(bp);
	rc = bnx2x_phy_write(&bp->link_params, prtad, devad, addr, value);
	bnx2x_release_phy_lock(bp);
	return rc;
}