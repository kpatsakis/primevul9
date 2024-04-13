static int do_memory_failure(struct mce *m)
{
	int flags = MF_ACTION_REQUIRED;
	int ret;

	pr_err("Uncorrected hardware memory error in user-access at %llx", m->addr);
	if (!(m->mcgstatus & MCG_STATUS_RIPV))
		flags |= MF_MUST_KILL;
	ret = memory_failure(m->addr >> PAGE_SHIFT, flags);
	if (ret)
		pr_err("Memory error not recovered");
	else
		mce_unmap_kpfn(m->addr >> PAGE_SHIFT);
	return ret;
}