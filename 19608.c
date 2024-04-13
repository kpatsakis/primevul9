static void iwl_trans_pcie_fw_alive(struct iwl_trans *trans, u32 scd_addr)
{
	iwl_pcie_reset_ict(trans);
	iwl_pcie_tx_start(trans, scd_addr);
}