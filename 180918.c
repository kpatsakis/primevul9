int numa_zonelist_order_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *length,
		loff_t *ppos)
{
	char *str;
	int ret;

	if (!write)
		return proc_dostring(table, write, buffer, length, ppos);
	str = memdup_user_nul(buffer, 16);
	if (IS_ERR(str))
		return PTR_ERR(str);

	ret = __parse_numa_zonelist_order(str);
	kfree(str);
	return ret;
}