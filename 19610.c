static ssize_t iwl_dbgfs_rfkill_write(struct file *file,
				      const char __user *user_buf,
				      size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	bool new_value;
	int ret;

	ret = kstrtobool_from_user(user_buf, count, &new_value);
	if (ret)
		return ret;
	if (new_value == trans_pcie->debug_rfkill)
		return count;
	IWL_WARN(trans, "changing debug rfkill %d->%d\n",
		 trans_pcie->debug_rfkill, new_value);
	trans_pcie->debug_rfkill = new_value;
	iwl_pcie_handle_rfkill_irq(trans);

	return count;
}