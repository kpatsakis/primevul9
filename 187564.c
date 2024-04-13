static int sas_ex_discover_devices(struct domain_device *dev, int single)
{
	struct expander_device *ex = &dev->ex_dev;
	int i = 0, end = ex->num_phys;
	int res = 0;

	if (0 <= single && single < end) {
		i = single;
		end = i+1;
	}

	for ( ; i < end; i++) {
		struct ex_phy *ex_phy = &ex->ex_phy[i];

		if (ex_phy->phy_state == PHY_VACANT ||
		    ex_phy->phy_state == PHY_NOT_PRESENT ||
		    ex_phy->phy_state == PHY_DEVICE_DISCOVERED)
			continue;

		switch (ex_phy->linkrate) {
		case SAS_PHY_DISABLED:
		case SAS_PHY_RESET_PROBLEM:
		case SAS_SATA_PORT_SELECTOR:
			continue;
		default:
			res = sas_ex_discover_dev(dev, i);
			if (res)
				break;
			continue;
		}
	}

	if (!res)
		sas_check_level_subtractive_boundary(dev);

	return res;
}