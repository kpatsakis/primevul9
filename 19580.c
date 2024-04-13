void iwl_trans_pcie_dbgfs_register(struct iwl_trans *trans)
{
	struct dentry *dir = trans->dbgfs_dir;

	DEBUGFS_ADD_FILE(rx_queue, dir, 0400);
	DEBUGFS_ADD_FILE(tx_queue, dir, 0400);
	DEBUGFS_ADD_FILE(interrupt, dir, 0600);
	DEBUGFS_ADD_FILE(csr, dir, 0200);
	DEBUGFS_ADD_FILE(fh_reg, dir, 0400);
	DEBUGFS_ADD_FILE(rfkill, dir, 0600);
	DEBUGFS_ADD_FILE(monitor_data, dir, 0400);
}