void i40e_irq_dynamic_disable_icr0(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;

	wr32(hw, I40E_PFINT_DYN_CTL0,
	     I40E_ITR_NONE << I40E_PFINT_DYN_CTLN_ITR_INDX_SHIFT);
	i40e_flush(hw);
}