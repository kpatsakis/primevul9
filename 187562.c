static int sas_rediscover_dev(struct domain_device *dev, int phy_id,
			      bool last, int sibling)
{
	struct expander_device *ex = &dev->ex_dev;
	struct ex_phy *phy = &ex->ex_phy[phy_id];
	enum sas_device_type type = SAS_PHY_UNUSED;
	u8 sas_addr[SAS_ADDR_SIZE];
	char msg[80] = "";
	int res;

	if (!last)
		sprintf(msg, ", part of a wide port with phy%02d", sibling);

	pr_debug("ex %016llx rediscovering phy%02d%s\n",
		 SAS_ADDR(dev->sas_addr), phy_id, msg);

	memset(sas_addr, 0, SAS_ADDR_SIZE);
	res = sas_get_phy_attached_dev(dev, phy_id, sas_addr, &type);
	switch (res) {
	case SMP_RESP_NO_PHY:
		phy->phy_state = PHY_NOT_PRESENT;
		sas_unregister_devs_sas_addr(dev, phy_id, last);
		return res;
	case SMP_RESP_PHY_VACANT:
		phy->phy_state = PHY_VACANT;
		sas_unregister_devs_sas_addr(dev, phy_id, last);
		return res;
	case SMP_RESP_FUNC_ACC:
		break;
	case -ECOMM:
		break;
	default:
		return res;
	}

	if ((SAS_ADDR(sas_addr) == 0) || (res == -ECOMM)) {
		phy->phy_state = PHY_EMPTY;
		sas_unregister_devs_sas_addr(dev, phy_id, last);
		/*
		 * Even though the PHY is empty, for convenience we discover
		 * the PHY to update the PHY info, like negotiated linkrate.
		 */
		sas_ex_phy_discover(dev, phy_id);
		return res;
	} else if (SAS_ADDR(sas_addr) == SAS_ADDR(phy->attached_sas_addr) &&
		   dev_type_flutter(type, phy->attached_dev_type)) {
		struct domain_device *ata_dev = sas_ex_to_ata(dev, phy_id);
		char *action = "";

		sas_ex_phy_discover(dev, phy_id);

		if (ata_dev && phy->attached_dev_type == SAS_SATA_PENDING)
			action = ", needs recovery";
		pr_debug("ex %016llx phy%02d broadcast flutter%s\n",
			 SAS_ADDR(dev->sas_addr), phy_id, action);
		return res;
	}

	/* we always have to delete the old device when we went here */
	pr_info("ex %016llx phy%02d replace %016llx\n",
		SAS_ADDR(dev->sas_addr), phy_id,
		SAS_ADDR(phy->attached_sas_addr));
	sas_unregister_devs_sas_addr(dev, phy_id, last);

	return sas_discover_new(dev, phy_id);
}