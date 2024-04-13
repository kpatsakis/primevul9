static int sas_discover_new(struct domain_device *dev, int phy_id)
{
	struct ex_phy *ex_phy = &dev->ex_dev.ex_phy[phy_id];
	struct domain_device *child;
	int res;

	pr_debug("ex %016llx phy%02d new device attached\n",
		 SAS_ADDR(dev->sas_addr), phy_id);
	res = sas_ex_phy_discover(dev, phy_id);
	if (res)
		return res;

	if (sas_ex_join_wide_port(dev, phy_id))
		return 0;

	res = sas_ex_discover_devices(dev, phy_id);
	if (res)
		return res;
	list_for_each_entry(child, &dev->ex_dev.children, siblings) {
		if (SAS_ADDR(child->sas_addr) ==
		    SAS_ADDR(ex_phy->attached_sas_addr)) {
			if (child->dev_type == SAS_EDGE_EXPANDER_DEVICE ||
			    child->dev_type == SAS_FANOUT_EXPANDER_DEVICE)
				res = sas_discover_bfs_by_root(child);
			break;
		}
	}
	return res;
}