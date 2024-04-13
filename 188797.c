static int i40e_pf_rxq_wait(struct i40e_pf *pf, int pf_q, bool enable)
{
	int i;
	u32 rx_reg;

	for (i = 0; i < I40E_QUEUE_WAIT_RETRY_LIMIT; i++) {
		rx_reg = rd32(&pf->hw, I40E_QRX_ENA(pf_q));
		if (enable == !!(rx_reg & I40E_QRX_ENA_QENA_STAT_MASK))
			break;

		usleep_range(10, 20);
	}
	if (i >= I40E_QUEUE_WAIT_RETRY_LIMIT)
		return -ETIMEDOUT;

	return 0;
}