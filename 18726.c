static int xemaclite_mdio_setup(struct net_local *lp, struct device *dev)
{
	struct mii_bus *bus;
	int rc;
	struct resource res;
	struct device_node *np = of_get_parent(lp->phy_node);
	struct device_node *npp;

	/* Don't register the MDIO bus if the phy_node or its parent node
	 * can't be found.
	 */
	if (!np) {
		dev_err(dev, "Failed to register mdio bus.\n");
		return -ENODEV;
	}
	npp = of_get_parent(np);

	of_address_to_resource(npp, 0, &res);
	if (lp->ndev->mem_start != res.start) {
		struct phy_device *phydev;
		phydev = of_phy_find_device(lp->phy_node);
		if (!phydev)
			dev_info(dev,
				 "MDIO of the phy is not registered yet\n");
		else
			put_device(&phydev->mdio.dev);
		return 0;
	}

	/* Enable the MDIO bus by asserting the enable bit in MDIO Control
	 * register.
	 */
	xemaclite_writel(XEL_MDIOCTRL_MDIOEN_MASK,
			 lp->base_addr + XEL_MDIOCTRL_OFFSET);

	bus = mdiobus_alloc();
	if (!bus) {
		dev_err(dev, "Failed to allocate mdiobus\n");
		return -ENOMEM;
	}

	snprintf(bus->id, MII_BUS_ID_SIZE, "%.8llx",
		 (unsigned long long)res.start);
	bus->priv = lp;
	bus->name = "Xilinx Emaclite MDIO";
	bus->read = xemaclite_mdio_read;
	bus->write = xemaclite_mdio_write;
	bus->parent = dev;

	rc = of_mdiobus_register(bus, np);
	if (rc) {
		dev_err(dev, "Failed to register mdio bus.\n");
		goto err_register;
	}

	lp->mii_bus = bus;

	return 0;

err_register:
	mdiobus_free(bus);
	return rc;
}