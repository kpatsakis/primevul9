static int i40e_vsi_control_rx(struct i40e_vsi *vsi, bool enable)
{
	struct i40e_pf *pf = vsi->back;
	int i, pf_q, ret = 0;

	pf_q = vsi->base_queue;
	for (i = 0; i < vsi->num_queue_pairs; i++, pf_q++) {
		ret = i40e_control_wait_rx_q(pf, pf_q, enable);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "VSI seid %d Rx ring %d %sable timeout\n",
				 vsi->seid, pf_q, (enable ? "en" : "dis"));
			break;
		}
	}

	/* Due to HW errata, on Rx disable only, the register can indicate done
	 * before it really is. Needs 50ms to be sure
	 */
	if (!enable)
		mdelay(50);

	return ret;
}