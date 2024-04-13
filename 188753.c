void i40e_irq_dynamic_enable_icr0(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	u32 val;

	val = I40E_PFINT_DYN_CTL0_INTENA_MASK   |
	      I40E_PFINT_DYN_CTL0_CLEARPBA_MASK |
	      (I40E_ITR_NONE << I40E_PFINT_DYN_CTL0_ITR_INDX_SHIFT);

	wr32(hw, I40E_PFINT_DYN_CTL0, val);
	i40e_flush(hw);
}