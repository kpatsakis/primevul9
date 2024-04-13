static void iwl_pcie_map_non_rx_causes(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie =  IWL_TRANS_GET_PCIE_TRANS(trans);
	int val = trans_pcie->def_irq | MSIX_NON_AUTO_CLEAR_CAUSE;
	int i, arr_size =
		(trans->trans_cfg->device_family != IWL_DEVICE_FAMILY_22560) ?
		ARRAY_SIZE(causes_list) : ARRAY_SIZE(causes_list_v2);

	/*
	 * Access all non RX causes and map them to the default irq.
	 * In case we are missing at least one interrupt vector,
	 * the first interrupt vector will serve non-RX and FBQ causes.
	 */
	for (i = 0; i < arr_size; i++) {
		struct iwl_causes_list *causes =
			(trans->trans_cfg->device_family !=
			 IWL_DEVICE_FAMILY_22560) ?
			causes_list : causes_list_v2;

		iwl_write8(trans, CSR_MSIX_IVAR(causes[i].addr), val);
		iwl_clear_bit(trans, causes[i].mask_reg,
			      causes[i].cause_num);
	}
}