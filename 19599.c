static void iwl_trans_pcie_release_nic_access(struct iwl_trans *trans,
					      unsigned long *flags)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	lockdep_assert_held(&trans_pcie->reg_lock);

	/*
	 * Fool sparse by faking we acquiring the lock - sparse will
	 * track nic_access anyway.
	 */
	__acquire(&trans_pcie->reg_lock);

	if (trans_pcie->cmd_hold_nic_awake)
		goto out;

	__iwl_trans_pcie_clear_bit(trans, CSR_GP_CNTRL,
				   BIT(trans->trans_cfg->csr->flag_mac_access_req));
	/*
	 * Above we read the CSR_GP_CNTRL register, which will flush
	 * any previous writes, but we need the write that clears the
	 * MAC_ACCESS_REQ bit to be performed before any other writes
	 * scheduled on different CPUs (after we drop reg_lock).
	 */
out:
	spin_unlock_irqrestore(&trans_pcie->reg_lock, *flags);
}