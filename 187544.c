static int sas_discover_bfs_by_root(struct domain_device *dev)
{
	int res;
	struct sas_expander_device *ex = rphy_to_expander_device(dev->rphy);
	int level = ex->level+1;

	res = sas_ex_discover_devices(dev, -1);
	if (res)
		goto out;
	do {
		res = sas_discover_bfs_by_root_level(dev, level);
		mb();
		level += 1;
	} while (level <= dev->port->disc.max_level);
out:
	return res;
}