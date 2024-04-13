static void sas_ex_get_linkrate(struct domain_device *parent,
				       struct domain_device *child,
				       struct ex_phy *parent_phy)
{
	struct expander_device *parent_ex = &parent->ex_dev;
	struct sas_port *port;
	int i;

	child->pathways = 0;

	port = parent_phy->port;

	for (i = 0; i < parent_ex->num_phys; i++) {
		struct ex_phy *phy = &parent_ex->ex_phy[i];

		if (phy->phy_state == PHY_VACANT ||
		    phy->phy_state == PHY_NOT_PRESENT)
			continue;

		if (SAS_ADDR(phy->attached_sas_addr) ==
		    SAS_ADDR(child->sas_addr)) {

			child->min_linkrate = min(parent->min_linkrate,
						  phy->linkrate);
			child->max_linkrate = max(parent->max_linkrate,
						  phy->linkrate);
			child->pathways++;
			sas_port_add_phy(port, phy->phy);
		}
	}
	child->linkrate = min(parent_phy->linkrate, child->max_linkrate);
	child->pathways = min(child->pathways, parent->pathways);
}