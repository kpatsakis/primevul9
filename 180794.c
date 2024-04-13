int sysctl_min_unmapped_ratio_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	int rc;

	rc = proc_dointvec_minmax(table, write, buffer, length, ppos);
	if (rc)
		return rc;

	setup_min_unmapped_ratio();

	return 0;
}