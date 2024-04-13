int memory_failure(unsigned long pfn, int flags)
{
	/* mce_severity() should not hand us an ACTION_REQUIRED error */
	BUG_ON(flags & MF_ACTION_REQUIRED);
	pr_err("Uncorrected memory error in page 0x%lx ignored\n"
	       "Rebuild kernel with CONFIG_MEMORY_FAILURE=y for smarter handling\n",
	       pfn);

	return 0;
}