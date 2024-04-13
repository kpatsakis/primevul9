static int sas_ex_phy_discover_helper(struct domain_device *dev, u8 *disc_req,
				      u8 *disc_resp, int single)
{
	struct discover_resp *dr;
	int res;

	disc_req[9] = single;

	res = smp_execute_task(dev, disc_req, DISCOVER_REQ_SIZE,
			       disc_resp, DISCOVER_RESP_SIZE);
	if (res)
		return res;
	dr = &((struct smp_resp *)disc_resp)->disc;
	if (memcmp(dev->sas_addr, dr->attached_sas_addr, SAS_ADDR_SIZE) == 0) {
		pr_notice("Found loopback topology, just ignore it!\n");
		return 0;
	}
	sas_set_ex_phy(dev, single, disc_resp);
	return 0;
}