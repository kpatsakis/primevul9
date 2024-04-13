static void i40e_enable_misc_int_causes(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	u32 val;

	/* clear things first */
	wr32(hw, I40E_PFINT_ICR0_ENA, 0);  /* disable all */
	rd32(hw, I40E_PFINT_ICR0);         /* read to clear */

	val = I40E_PFINT_ICR0_ENA_ECC_ERR_MASK       |
	      I40E_PFINT_ICR0_ENA_MAL_DETECT_MASK    |
	      I40E_PFINT_ICR0_ENA_GRST_MASK          |
	      I40E_PFINT_ICR0_ENA_PCI_EXCEPTION_MASK |
	      I40E_PFINT_ICR0_ENA_GPIO_MASK          |
	      I40E_PFINT_ICR0_ENA_HMC_ERR_MASK       |
	      I40E_PFINT_ICR0_ENA_VFLR_MASK          |
	      I40E_PFINT_ICR0_ENA_ADMINQ_MASK;

	if (pf->flags & I40E_FLAG_IWARP_ENABLED)
		val |= I40E_PFINT_ICR0_ENA_PE_CRITERR_MASK;

	if (pf->flags & I40E_FLAG_PTP)
		val |= I40E_PFINT_ICR0_ENA_TIMESYNC_MASK;

	wr32(hw, I40E_PFINT_ICR0_ENA, val);

	/* SW_ITR_IDX = 0, but don't change INTENA */
	wr32(hw, I40E_PFINT_DYN_CTL0, I40E_PFINT_DYN_CTL0_SW_ITR_INDX_MASK |
					I40E_PFINT_DYN_CTL0_INTENA_MSK_MASK);

	/* OTHER_ITR_IDX = 0 */
	wr32(hw, I40E_PFINT_STAT_CTL0, 0);
}