int sas_discover_root_expander(struct domain_device *dev)
{
	int res;
	struct sas_expander_device *ex = rphy_to_expander_device(dev->rphy);

	res = sas_rphy_add(dev->rphy);
	if (res)
		goto out_err;

	ex->level = dev->port->disc.max_level; /* 0 */
	res = sas_discover_expander(dev);
	if (res)
		goto out_err2;

	sas_ex_bfs_disc(dev->port);

	return res;

out_err2:
	sas_rphy_remove(dev->rphy);
out_err:
	return res;
}