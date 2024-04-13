int min_free_kbytes_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	int rc;

	rc = proc_dointvec_minmax(table, write, buffer, length, ppos);
	if (rc)
		return rc;

	if (write) {
		user_min_free_kbytes = min_free_kbytes;
		setup_per_zone_wmarks();
	}
	return 0;
}