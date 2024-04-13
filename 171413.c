
static void ql_get_drvinfo(struct net_device *ndev,
			   struct ethtool_drvinfo *drvinfo)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	strlcpy(drvinfo->driver, ql3xxx_driver_name, sizeof(drvinfo->driver));
	strlcpy(drvinfo->version, ql3xxx_driver_version,
		sizeof(drvinfo->version));
	strlcpy(drvinfo->bus_info, pci_name(qdev->pdev),
		sizeof(drvinfo->bus_info));