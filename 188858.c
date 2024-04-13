int i40e_control_wait_tx_q(int seid, struct i40e_pf *pf, int pf_q,
			   bool is_xdp, bool enable)
{
	int ret;

	i40e_control_tx_q(pf, pf_q, enable);

	/* wait for the change to finish */
	ret = i40e_pf_txq_wait(pf, pf_q, enable);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "VSI seid %d %sTx ring %d %sable timeout\n",
			 seid, (is_xdp ? "XDP " : ""), pf_q,
			 (enable ? "en" : "dis"));
	}

	return ret;
}