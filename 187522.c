static int sas_get_phy_discover(struct domain_device *dev,
				int phy_id, struct smp_resp *disc_resp)
{
	int res;
	u8 *disc_req;

	disc_req = alloc_smp_req(DISCOVER_REQ_SIZE);
	if (!disc_req)
		return -ENOMEM;

	disc_req[1] = SMP_DISCOVER;
	disc_req[9] = phy_id;

	res = smp_execute_task(dev, disc_req, DISCOVER_REQ_SIZE,
			       disc_resp, DISCOVER_RESP_SIZE);
	if (res)
		goto out;
	else if (disc_resp->result != SMP_RESP_FUNC_ACC) {
		res = disc_resp->result;
		goto out;
	}
out:
	kfree(disc_req);
	return res;
}