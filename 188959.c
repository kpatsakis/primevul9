static void i40e_control_rx_q(struct i40e_pf *pf, int pf_q, bool enable)
{
	struct i40e_hw *hw = &pf->hw;
	u32 rx_reg;
	int i;

	for (i = 0; i < I40E_QTX_ENA_WAIT_COUNT; i++) {
		rx_reg = rd32(hw, I40E_QRX_ENA(pf_q));
		if (((rx_reg >> I40E_QRX_ENA_QENA_REQ_SHIFT) & 1) ==
		    ((rx_reg >> I40E_QRX_ENA_QENA_STAT_SHIFT) & 1))
			break;
		usleep_range(1000, 2000);
	}

	/* Skip if the queue is already in the requested state */
	if (enable == !!(rx_reg & I40E_QRX_ENA_QENA_STAT_MASK))
		return;

	/* turn on/off the queue */
	if (enable)
		rx_reg |= I40E_QRX_ENA_QENA_REQ_MASK;
	else
		rx_reg &= ~I40E_QRX_ENA_QENA_REQ_MASK;

	wr32(hw, I40E_QRX_ENA(pf_q), rx_reg);
}