static int bnx2x_mdio_read(struct net_device *netdev, int prtad,
			   int devad, u16 addr)
{
	struct bnx2x *bp = netdev_priv(netdev);
	u16 value;
	int rc;

	DP(NETIF_MSG_LINK, "mdio_read: prtad 0x%x, devad 0x%x, addr 0x%x\n",
	   prtad, devad, addr);

	/* The HW expects different devad if CL22 is used */
	devad = (devad == MDIO_DEVAD_NONE) ? DEFAULT_PHY_DEV_ADDR : devad;

	bnx2x_acquire_phy_lock(bp);
	rc = bnx2x_phy_read(&bp->link_params, prtad, devad, addr, &value);
	bnx2x_release_phy_lock(bp);
	DP(NETIF_MSG_LINK, "mdio_read_val 0x%x rc = 0x%x\n", value, rc);

	if (!rc)
		rc = value;
	return rc;
}