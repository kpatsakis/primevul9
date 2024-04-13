static bool sas_ex_join_wide_port(struct domain_device *parent, int phy_id)
{
	struct ex_phy *phy = &parent->ex_dev.ex_phy[phy_id];
	int i;

	for (i = 0; i < parent->ex_dev.num_phys; i++) {
		struct ex_phy *ephy = &parent->ex_dev.ex_phy[i];

		if (ephy == phy)
			continue;

		if (!memcmp(phy->attached_sas_addr, ephy->attached_sas_addr,
			    SAS_ADDR_SIZE) && ephy->port) {
			sas_port_add_phy(ephy->port, phy->phy);
			phy->port = ephy->port;
			phy->phy_state = PHY_DEVICE_DISCOVERED;
			return true;
		}
	}

	return false;
}