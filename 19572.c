static int iwl_dbgfs_monitor_data_release(struct inode *inode,
					  struct file *file)
{
	struct iwl_trans_pcie *trans_pcie =
		IWL_TRANS_GET_PCIE_TRANS(inode->i_private);

	if (trans_pcie->fw_mon_data.state == IWL_FW_MON_DBGFS_STATE_OPEN)
		trans_pcie->fw_mon_data.state = IWL_FW_MON_DBGFS_STATE_CLOSED;
	return 0;
}