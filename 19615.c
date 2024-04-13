void iwl_trans_pcie_sync_nmi(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	unsigned long timeout = jiffies + IWL_TRANS_NMI_TIMEOUT;
	bool interrupts_enabled = test_bit(STATUS_INT_ENABLED, &trans->status);
	u32 inta_addr, sw_err_bit;

	if (trans_pcie->msix_enabled) {
		inta_addr = CSR_MSIX_HW_INT_CAUSES_AD;
		sw_err_bit = MSIX_HW_INT_CAUSES_REG_SW_ERR;
	} else {
		inta_addr = CSR_INT;
		sw_err_bit = CSR_INT_BIT_SW_ERR;
	}

	/* if the interrupts were already disabled, there is no point in
	 * calling iwl_disable_interrupts
	 */
	if (interrupts_enabled)
		iwl_disable_interrupts(trans);

	iwl_force_nmi(trans);
	while (time_after(timeout, jiffies)) {
		u32 inta_hw = iwl_read32(trans, inta_addr);

		/* Error detected by uCode */
		if (inta_hw & sw_err_bit) {
			/* Clear causes register */
			iwl_write32(trans, inta_addr, inta_hw & sw_err_bit);
			break;
		}

		mdelay(1);
	}

	/* enable interrupts only if there were already enabled before this
	 * function to avoid a case were the driver enable interrupts before
	 * proper configurations were made
	 */
	if (interrupts_enabled)
		iwl_enable_interrupts(trans);

	iwl_trans_fw_error(trans);
}