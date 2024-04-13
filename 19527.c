static ssize_t iwl_dbgfs_interrupt_write(struct file *file,
					 const char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct isr_statistics *isr_stats = &trans_pcie->isr_stats;
	u32 reset_flag;
	int ret;

	ret = kstrtou32_from_user(user_buf, count, 16, &reset_flag);
	if (ret)
		return ret;
	if (reset_flag == 0)
		memset(isr_stats, 0, sizeof(*isr_stats));

	return count;
}