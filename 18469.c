static int proc_do_entropy(struct ctl_table *table, int write,
			   void *buffer, size_t *lenp, loff_t *ppos)
{
	struct ctl_table fake_table;
	int entropy_count;

	entropy_count = *(int *)table->data >> ENTROPY_SHIFT;

	fake_table.data = &entropy_count;
	fake_table.maxlen = sizeof(entropy_count);

	return proc_dointvec(&fake_table, write, buffer, lenp, ppos);
}