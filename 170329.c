static void bpf_prog_show_fdinfo(struct seq_file *m, struct file *filp)
{
	const struct bpf_prog *prog = filp->private_data;
	char prog_tag[sizeof(prog->tag) * 2 + 1] = { };
	struct bpf_prog_stats stats;

	bpf_prog_get_stats(prog, &stats);
	bin2hex(prog_tag, prog->tag, sizeof(prog->tag));
	seq_printf(m,
		   "prog_type:\t%u\n"
		   "prog_jited:\t%u\n"
		   "prog_tag:\t%s\n"
		   "memlock:\t%llu\n"
		   "prog_id:\t%u\n"
		   "run_time_ns:\t%llu\n"
		   "run_cnt:\t%llu\n"
		   "recursion_misses:\t%llu\n",
		   prog->type,
		   prog->jited,
		   prog_tag,
		   prog->pages * 1ULL << PAGE_SHIFT,
		   prog->aux->id,
		   stats.nsecs,
		   stats.cnt,
		   stats.misses);
}