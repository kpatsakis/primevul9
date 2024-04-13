static int sas_get_ex_change_count(struct domain_device *dev, int *ecc)
{
	int res;
	u8  *rg_req;
	struct smp_resp  *rg_resp;

	rg_req = alloc_smp_req(RG_REQ_SIZE);
	if (!rg_req)
		return -ENOMEM;

	rg_resp = alloc_smp_resp(RG_RESP_SIZE);
	if (!rg_resp) {
		kfree(rg_req);
		return -ENOMEM;
	}

	rg_req[1] = SMP_REPORT_GENERAL;

	res = smp_execute_task(dev, rg_req, RG_REQ_SIZE, rg_resp,
			       RG_RESP_SIZE);
	if (res)
		goto out;
	if (rg_resp->result != SMP_RESP_FUNC_ACC) {
		res = rg_resp->result;
		goto out;
	}

	*ecc = be16_to_cpu(rg_resp->rg.change_count);
out:
	kfree(rg_resp);
	kfree(rg_req);
	return res;
}