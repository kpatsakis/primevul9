static int sas_discover_bfs_by_root_level(struct domain_device *root,
					  const int level)
{
	struct expander_device *ex_root = &root->ex_dev;
	struct domain_device *child;
	int res = 0;

	list_for_each_entry(child, &ex_root->children, siblings) {
		if (child->dev_type == SAS_EDGE_EXPANDER_DEVICE ||
		    child->dev_type == SAS_FANOUT_EXPANDER_DEVICE) {
			struct sas_expander_device *ex =
				rphy_to_expander_device(child->rphy);

			if (level > ex->level)
				res = sas_discover_bfs_by_root_level(child,
								     level);
			else if (level == ex->level)
				res = sas_ex_discover_devices(child, -1);
		}
	}
	return res;
}