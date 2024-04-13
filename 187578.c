static int sas_ex_general(struct domain_device *dev)
{
	u8 *rg_req;
	struct smp_resp *rg_resp;
	int res;
	int i;

	rg_req = alloc_smp_req(RG_REQ_SIZE);
	if (!rg_req)
		return -ENOMEM;

	rg_resp = alloc_smp_resp(RG_RESP_SIZE);
	if (!rg_resp) {
		kfree(rg_req);
		return -ENOMEM;
	}

	rg_req[1] = SMP_REPORT_GENERAL;

	for (i = 0; i < 5; i++) {
		res = smp_execute_task(dev, rg_req, RG_REQ_SIZE, rg_resp,
				       RG_RESP_SIZE);

		if (res) {
			pr_notice("RG to ex %016llx failed:0x%x\n",
				  SAS_ADDR(dev->sas_addr), res);
			goto out;
		} else if (rg_resp->result != SMP_RESP_FUNC_ACC) {
			pr_debug("RG:ex %016llx returned SMP result:0x%x\n",
				 SAS_ADDR(dev->sas_addr), rg_resp->result);
			res = rg_resp->result;
			goto out;
		}

		ex_assign_report_general(dev, rg_resp);

		if (dev->ex_dev.configuring) {
			pr_debug("RG: ex %llx self-configuring...\n",
				 SAS_ADDR(dev->sas_addr));
			schedule_timeout_interruptible(5*HZ);
		} else
			break;
	}
out:
	kfree(rg_req);
	kfree(rg_resp);
	return res;
}