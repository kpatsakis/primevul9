static int temac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct temac_local *lp = netdev_priv(ndev);

	unregister_netdev(ndev);
	sysfs_remove_group(&lp->dev->kobj, &temac_attr_group);
	if (lp->phy_node)
		of_node_put(lp->phy_node);
	temac_mdio_teardown(lp);
	return 0;
}