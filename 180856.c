int lowmem_reserve_ratio_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	proc_dointvec_minmax(table, write, buffer, length, ppos);
	setup_per_zone_lowmem_reserve();
	return 0;
}