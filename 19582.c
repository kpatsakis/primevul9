static void iwl_pcie_init_msix(struct iwl_trans_pcie *trans_pcie)
{
	struct iwl_trans *trans = trans_pcie->trans;

	iwl_pcie_conf_msix_hw(trans_pcie);

	if (!trans_pcie->msix_enabled)
		return;

	trans_pcie->fh_init_mask = ~iwl_read32(trans, CSR_MSIX_FH_INT_MASK_AD);
	trans_pcie->fh_mask = trans_pcie->fh_init_mask;
	trans_pcie->hw_init_mask = ~iwl_read32(trans, CSR_MSIX_HW_INT_MASK_AD);
	trans_pcie->hw_mask = trans_pcie->hw_init_mask;
}