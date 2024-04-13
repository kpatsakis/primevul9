int i40e_control_wait_rx_q(struct i40e_pf *pf, int pf_q, bool enable)
{
	int ret = 0;

	i40e_control_rx_q(pf, pf_q, enable);

	/* wait for the change to finish */
	ret = i40e_pf_rxq_wait(pf, pf_q, enable);
	if (ret)
		return ret;

	return ret;
}