static void i40e_control_tx_q(struct i40e_pf *pf, int pf_q, bool enable)
{
	struct i40e_hw *hw = &pf->hw;
	u32 tx_reg;
	int i;

	/* warn the TX unit of coming changes */
	i40e_pre_tx_queue_cfg(&pf->hw, pf_q, enable);
	if (!enable)
		usleep_range(10, 20);

	for (i = 0; i < I40E_QTX_ENA_WAIT_COUNT; i++) {
		tx_reg = rd32(hw, I40E_QTX_ENA(pf_q));
		if (((tx_reg >> I40E_QTX_ENA_QENA_REQ_SHIFT) & 1) ==
		    ((tx_reg >> I40E_QTX_ENA_QENA_STAT_SHIFT) & 1))
			break;
		usleep_range(1000, 2000);
	}

	/* Skip if the queue is already in the requested state */
	if (enable == !!(tx_reg & I40E_QTX_ENA_QENA_STAT_MASK))
		return;

	/* turn on/off the queue */
	if (enable) {
		wr32(hw, I40E_QTX_HEAD(pf_q), 0);
		tx_reg |= I40E_QTX_ENA_QENA_REQ_MASK;
	} else {
		tx_reg &= ~I40E_QTX_ENA_QENA_REQ_MASK;
	}

	wr32(hw, I40E_QTX_ENA(pf_q), tx_reg);
}