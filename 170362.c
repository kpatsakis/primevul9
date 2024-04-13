static void bpf_tracing_link_show_fdinfo(const struct bpf_link *link,
					 struct seq_file *seq)
{
	struct bpf_tracing_link *tr_link =
		container_of(link, struct bpf_tracing_link, link);

	seq_printf(seq,
		   "attach_type:\t%d\n",
		   tr_link->attach_type);
}