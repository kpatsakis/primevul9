void warn_alloc(gfp_t gfp_mask, nodemask_t *nodemask, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;
	static DEFINE_RATELIMIT_STATE(nopage_rs, DEFAULT_RATELIMIT_INTERVAL,
				      DEFAULT_RATELIMIT_BURST);

	if ((gfp_mask & __GFP_NOWARN) || !__ratelimit(&nopage_rs))
		return;

	pr_warn("%s: ", current->comm);

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	pr_cont("%pV", &vaf);
	va_end(args);

	pr_cont(", mode:%#x(%pGg), nodemask=", gfp_mask, &gfp_mask);
	if (nodemask)
		pr_cont("%*pbl\n", nodemask_pr_args(nodemask));
	else
		pr_cont("(null)\n");

	cpuset_print_current_mems_allowed();

	dump_stack();
	warn_alloc_show_mem(gfp_mask, nodemask);
}