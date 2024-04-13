static ssize_t iwl_dbgfs_fh_reg_read(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	char *buf = NULL;
	ssize_t ret;

	ret = iwl_dump_fh(trans, &buf);
	if (ret < 0)
		return ret;
	if (!buf)
		return -EINVAL;
	ret = simple_read_from_buffer(user_buf, count, ppos, buf, ret);
	kfree(buf);
	return ret;
}