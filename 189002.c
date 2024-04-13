static int i40e_pf_txq_wait(struct i40e_pf *pf, int pf_q, bool enable)
{
	int i;
	u32 tx_reg;

	for (i = 0; i < I40E_QUEUE_WAIT_RETRY_LIMIT; i++) {
		tx_reg = rd32(&pf->hw, I40E_QTX_ENA(pf_q));
		if (enable == !!(tx_reg & I40E_QTX_ENA_QENA_STAT_MASK))
			break;

		usleep_range(10, 20);
	}
	if (i >= I40E_QUEUE_WAIT_RETRY_LIMIT)
		return -ETIMEDOUT;

	return 0;
}