static int sas_get_phy_change_count(struct domain_device *dev,
				    int phy_id, int *pcc)
{
	int res;
	struct smp_resp *disc_resp;

	disc_resp = alloc_smp_resp(DISCOVER_RESP_SIZE);
	if (!disc_resp)
		return -ENOMEM;

	res = sas_get_phy_discover(dev, phy_id, disc_resp);
	if (!res)
		*pcc = disc_resp->disc.change_count;

	kfree(disc_resp);
	return res;
}