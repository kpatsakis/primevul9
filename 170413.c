static void bpf_raw_tp_link_show_fdinfo(const struct bpf_link *link,
					struct seq_file *seq)
{
	struct bpf_raw_tp_link *raw_tp_link =
		container_of(link, struct bpf_raw_tp_link, link);

	seq_printf(seq,
		   "tp_name:\t%s\n",
		   raw_tp_link->btp->tp->name);
}