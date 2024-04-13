static ssize_t iwl_dbgfs_rfkill_read(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	char buf[100];
	int pos;

	pos = scnprintf(buf, sizeof(buf), "debug: %d\nhw: %d\n",
			trans_pcie->debug_rfkill,
			!(iwl_read32(trans, CSR_GP_CNTRL) &
				CSR_GP_CNTRL_REG_FLAG_HW_RF_KILL_SW));

	return simple_read_from_buffer(user_buf, count, ppos, buf, pos);
}