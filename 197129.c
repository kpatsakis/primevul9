static void bond_ethtool_get_drvinfo(struct net_device *bond_dev,
				     struct ethtool_drvinfo *drvinfo)
{
	strlcpy(drvinfo->driver, DRV_NAME, sizeof(drvinfo->driver));
	snprintf(drvinfo->fw_version, sizeof(drvinfo->fw_version), "%d",
		 BOND_ABI_VERSION);
}