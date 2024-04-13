static int sas_expander_discover(struct domain_device *dev)
{
	struct expander_device *ex = &dev->ex_dev;
	int res = -ENOMEM;

	ex->ex_phy = kcalloc(ex->num_phys, sizeof(*ex->ex_phy), GFP_KERNEL);
	if (!ex->ex_phy)
		return -ENOMEM;

	res = sas_ex_phy_discover(dev, -1);
	if (res)
		goto out_err;

	return 0;
 out_err:
	kfree(ex->ex_phy);
	ex->ex_phy = NULL;
	return res;
}