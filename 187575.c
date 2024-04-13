static int sas_check_parent_topology(struct domain_device *child)
{
	struct expander_device *child_ex = &child->ex_dev;
	struct expander_device *parent_ex;
	int i;
	int res = 0;

	if (!child->parent)
		return 0;

	if (child->parent->dev_type != SAS_EDGE_EXPANDER_DEVICE &&
	    child->parent->dev_type != SAS_FANOUT_EXPANDER_DEVICE)
		return 0;

	parent_ex = &child->parent->ex_dev;

	for (i = 0; i < parent_ex->num_phys; i++) {
		struct ex_phy *parent_phy = &parent_ex->ex_phy[i];
		struct ex_phy *child_phy;

		if (parent_phy->phy_state == PHY_VACANT ||
		    parent_phy->phy_state == PHY_NOT_PRESENT)
			continue;

		if (SAS_ADDR(parent_phy->attached_sas_addr) != SAS_ADDR(child->sas_addr))
			continue;

		child_phy = &child_ex->ex_phy[parent_phy->attached_phy_id];

		switch (child->parent->dev_type) {
		case SAS_EDGE_EXPANDER_DEVICE:
			if (child->dev_type == SAS_FANOUT_EXPANDER_DEVICE) {
				if (parent_phy->routing_attr != SUBTRACTIVE_ROUTING ||
				    child_phy->routing_attr != TABLE_ROUTING) {
					sas_print_parent_topology_bug(child, parent_phy, child_phy);
					res = -ENODEV;
				}
			} else if (parent_phy->routing_attr == SUBTRACTIVE_ROUTING) {
				if (child_phy->routing_attr == SUBTRACTIVE_ROUTING) {
					res = sas_check_eeds(child, parent_phy, child_phy);
				} else if (child_phy->routing_attr != TABLE_ROUTING) {
					sas_print_parent_topology_bug(child, parent_phy, child_phy);
					res = -ENODEV;
				}
			} else if (parent_phy->routing_attr == TABLE_ROUTING) {
				if (child_phy->routing_attr == SUBTRACTIVE_ROUTING ||
				    (child_phy->routing_attr == TABLE_ROUTING &&
				     child_ex->t2t_supp && parent_ex->t2t_supp)) {
					/* All good */;
				} else {
					sas_print_parent_topology_bug(child, parent_phy, child_phy);
					res = -ENODEV;
				}
			}
			break;
		case SAS_FANOUT_EXPANDER_DEVICE:
			if (parent_phy->routing_attr != TABLE_ROUTING ||
			    child_phy->routing_attr != SUBTRACTIVE_ROUTING) {
				sas_print_parent_topology_bug(child, parent_phy, child_phy);
				res = -ENODEV;
			}
			break;
		default:
			break;
		}
	}

	return res;
}