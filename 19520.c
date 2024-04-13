static void iwl_trans_pcie_op_mode_leave(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	mutex_lock(&trans_pcie->mutex);

	/* disable interrupts - don't enable HW RF kill interrupt */
	iwl_disable_interrupts(trans);

	iwl_pcie_apm_stop(trans, true);

	iwl_disable_interrupts(trans);

	iwl_pcie_disable_ict(trans);

	mutex_unlock(&trans_pcie->mutex);

	iwl_pcie_synchronize_irqs(trans);
}