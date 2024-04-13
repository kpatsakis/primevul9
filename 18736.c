static int xemaclite_mdio_read(struct mii_bus *bus, int phy_id, int reg)
{
	struct net_local *lp = bus->priv;
	u32 ctrl_reg;
	u32 rc;

	if (xemaclite_mdio_wait(lp))
		return -ETIMEDOUT;

	/* Write the PHY address, register number and set the OP bit in the
	 * MDIO Address register. Set the Status bit in the MDIO Control
	 * register to start a MDIO read transaction.
	 */
	ctrl_reg = xemaclite_readl(lp->base_addr + XEL_MDIOCTRL_OFFSET);
	xemaclite_writel(XEL_MDIOADDR_OP_MASK |
			 ((phy_id << XEL_MDIOADDR_PHYADR_SHIFT) | reg),
			 lp->base_addr + XEL_MDIOADDR_OFFSET);
	xemaclite_writel(ctrl_reg | XEL_MDIOCTRL_MDIOSTS_MASK,
			 lp->base_addr + XEL_MDIOCTRL_OFFSET);

	if (xemaclite_mdio_wait(lp))
		return -ETIMEDOUT;

	rc = xemaclite_readl(lp->base_addr + XEL_MDIORD_OFFSET);

	dev_dbg(&lp->ndev->dev,
		"%s(phy_id=%i, reg=%x) == %x\n", __func__,
		phy_id, reg, rc);

	return rc;
}