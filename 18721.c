static void xemaclite_ethtools_get_drvinfo(struct net_device *ndev,
					   struct ethtool_drvinfo *ed)
{
	strlcpy(ed->driver, DRIVER_NAME, sizeof(ed->driver));
}