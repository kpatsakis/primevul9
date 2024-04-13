int sas_get_report_phy_sata(struct domain_device *dev, int phy_id,
			    struct smp_resp *rps_resp)
{
	int res;
	u8 *rps_req = alloc_smp_req(RPS_REQ_SIZE);
	u8 *resp = (u8 *)rps_resp;

	if (!rps_req)
		return -ENOMEM;

	rps_req[1] = SMP_REPORT_PHY_SATA;
	rps_req[9] = phy_id;

	res = smp_execute_task(dev, rps_req, RPS_REQ_SIZE,
			            rps_resp, RPS_RESP_SIZE);

	/* 0x34 is the FIS type for the D2H fis.  There's a potential
	 * standards cockup here.  sas-2 explicitly specifies the FIS
	 * should be encoded so that FIS type is in resp[24].
	 * However, some expanders endian reverse this.  Undo the
	 * reversal here */
	if (!res && resp[27] == 0x34 && resp[24] != 0x34) {
		int i;

		for (i = 0; i < 5; i++) {
			int j = 24 + (i*4);
			u8 a, b;
			a = resp[j + 0];
			b = resp[j + 1];
			resp[j + 0] = resp[j + 3];
			resp[j + 1] = resp[j + 2];
			resp[j + 2] = b;
			resp[j + 3] = a;
		}
	}

	kfree(rps_req);
	return res;
}