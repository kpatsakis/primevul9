static void iwl_trans_pcie_debugfs_cleanup(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct cont_rec *data = &trans_pcie->fw_mon_data;

	mutex_lock(&data->mutex);
	data->state = IWL_FW_MON_DBGFS_STATE_DISABLED;
	mutex_unlock(&data->mutex);
}