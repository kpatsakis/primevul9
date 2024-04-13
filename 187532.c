static int sas_configure_set(struct domain_device *dev, int phy_id,
			     u8 *sas_addr, int index, int include)
{
	int res;
	u8 *cri_req;
	u8 *cri_resp;

	cri_req = alloc_smp_req(CRI_REQ_SIZE);
	if (!cri_req)
		return -ENOMEM;

	cri_resp = alloc_smp_resp(CRI_RESP_SIZE);
	if (!cri_resp) {
		kfree(cri_req);
		return -ENOMEM;
	}

	cri_req[1] = SMP_CONF_ROUTE_INFO;
	*(__be16 *)(cri_req+6) = cpu_to_be16(index);
	cri_req[9] = phy_id;
	if (SAS_ADDR(sas_addr) == 0 || !include)
		cri_req[12] |= 0x80;
	memcpy(cri_req+16, sas_addr, SAS_ADDR_SIZE);

	res = smp_execute_task(dev, cri_req, CRI_REQ_SIZE, cri_resp,
			       CRI_RESP_SIZE);
	if (res)
		goto out;
	res = cri_resp[2];
	if (res == SMP_RESP_NO_INDEX) {
		pr_warn("overflow of indexes: dev %016llx phy%02d index 0x%x\n",
			SAS_ADDR(dev->sas_addr), phy_id, index);
	}
out:
	kfree(cri_req);
	kfree(cri_resp);
	return res;
}