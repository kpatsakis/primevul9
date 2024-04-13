static int sas_configure_present(struct domain_device *dev, int phy_id,
				 u8 *sas_addr, int *index, int *present)
{
	int i, res = 0;
	struct expander_device *ex = &dev->ex_dev;
	struct ex_phy *phy = &ex->ex_phy[phy_id];
	u8 *rri_req;
	u8 *rri_resp;

	*present = 0;
	*index = 0;

	rri_req = alloc_smp_req(RRI_REQ_SIZE);
	if (!rri_req)
		return -ENOMEM;

	rri_resp = alloc_smp_resp(RRI_RESP_SIZE);
	if (!rri_resp) {
		kfree(rri_req);
		return -ENOMEM;
	}

	rri_req[1] = SMP_REPORT_ROUTE_INFO;
	rri_req[9] = phy_id;

	for (i = 0; i < ex->max_route_indexes ; i++) {
		*(__be16 *)(rri_req+6) = cpu_to_be16(i);
		res = smp_execute_task(dev, rri_req, RRI_REQ_SIZE, rri_resp,
				       RRI_RESP_SIZE);
		if (res)
			goto out;
		res = rri_resp[2];
		if (res == SMP_RESP_NO_INDEX) {
			pr_warn("overflow of indexes: dev %016llx phy%02d index 0x%x\n",
				SAS_ADDR(dev->sas_addr), phy_id, i);
			goto out;
		} else if (res != SMP_RESP_FUNC_ACC) {
			pr_notice("%s: dev %016llx phy%02d index 0x%x result 0x%x\n",
				  __func__, SAS_ADDR(dev->sas_addr), phy_id,
				  i, res);
			goto out;
		}
		if (SAS_ADDR(sas_addr) != 0) {
			if (SAS_ADDR(rri_resp+16) == SAS_ADDR(sas_addr)) {
				*index = i;
				if ((rri_resp[12] & 0x80) == 0x80)
					*present = 0;
				else
					*present = 1;
				goto out;
			} else if (SAS_ADDR(rri_resp+16) == 0) {
				*index = i;
				*present = 0;
				goto out;
			}
		} else if (SAS_ADDR(rri_resp+16) == 0 &&
			   phy->last_da_index < i) {
			phy->last_da_index = i;
			*index = i;
			*present = 0;
			goto out;
		}
	}
	res = -1;
out:
	kfree(rri_req);
	kfree(rri_resp);
	return res;
}