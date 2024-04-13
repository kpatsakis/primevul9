static int temac_open(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct phy_device *phydev = NULL;
	int rc;

	dev_dbg(&ndev->dev, "temac_open()\n");

	if (lp->phy_node) {
		phydev = of_phy_connect(lp->ndev, lp->phy_node,
					temac_adjust_link, 0, 0);
		if (!phydev) {
			dev_err(lp->dev, "of_phy_connect() failed\n");
			return -ENODEV;
		}
		phy_start(phydev);
	} else if (strlen(lp->phy_name) > 0) {
		phydev = phy_connect(lp->ndev, lp->phy_name, temac_adjust_link,
				     lp->phy_interface);
		if (IS_ERR(phydev)) {
			dev_err(lp->dev, "phy_connect() failed\n");
			return PTR_ERR(phydev);
		}
		phy_start(phydev);
	}

	temac_device_reset(ndev);

	rc = request_irq(lp->tx_irq, ll_temac_tx_irq, 0, ndev->name, ndev);
	if (rc)
		goto err_tx_irq;
	rc = request_irq(lp->rx_irq, ll_temac_rx_irq, 0, ndev->name, ndev);
	if (rc)
		goto err_rx_irq;

	return 0;

 err_rx_irq:
	free_irq(lp->tx_irq, ndev);
 err_tx_irq:
	if (phydev)
		phy_disconnect(phydev);
	dev_err(lp->dev, "request_irq() failed\n");
	return rc;
}