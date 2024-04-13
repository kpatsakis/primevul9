static int xemaclite_mdio_write(struct mii_bus *bus, int phy_id, int reg,
				u16 val)
{
	struct net_local *lp = bus->priv;
	u32 ctrl_reg;

	dev_dbg(&lp->ndev->dev,
		"%s(phy_id=%i, reg=%x, val=%x)\n", __func__,
		phy_id, reg, val);

	if (xemaclite_mdio_wait(lp))
		return -ETIMEDOUT;

	/* Write the PHY address, register number and clear the OP bit in the
	 * MDIO Address register and then write the value into the MDIO Write
	 * Data register. Finally, set the Status bit in the MDIO Control
	 * register to start a MDIO write transaction.
	 */
	ctrl_reg = xemaclite_readl(lp->base_addr + XEL_MDIOCTRL_OFFSET);
	xemaclite_writel(~XEL_MDIOADDR_OP_MASK &
			 ((phy_id << XEL_MDIOADDR_PHYADR_SHIFT) | reg),
			 lp->base_addr + XEL_MDIOADDR_OFFSET);
	xemaclite_writel(val, lp->base_addr + XEL_MDIOWR_OFFSET);
	xemaclite_writel(ctrl_reg | XEL_MDIOCTRL_MDIOSTS_MASK,
			 lp->base_addr + XEL_MDIOCTRL_OFFSET);

	return 0;
}