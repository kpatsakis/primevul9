static int iwl_dbgfs_monitor_data_open(struct inode *inode,
				       struct file *file)
{
	struct iwl_trans *trans = inode->i_private;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (!trans->dbg.dest_tlv ||
	    trans->dbg.dest_tlv->monitor_mode != EXTERNAL_MODE) {
		IWL_ERR(trans, "Debug destination is not set to DRAM\n");
		return -ENOENT;
	}

	if (trans_pcie->fw_mon_data.state != IWL_FW_MON_DBGFS_STATE_CLOSED)
		return -EBUSY;

	trans_pcie->fw_mon_data.state = IWL_FW_MON_DBGFS_STATE_OPEN;
	return simple_open(inode, file);
}