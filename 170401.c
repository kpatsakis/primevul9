static void bpf_link_show_fdinfo(struct seq_file *m, struct file *filp)
{
	const struct bpf_link *link = filp->private_data;
	const struct bpf_prog *prog = link->prog;
	char prog_tag[sizeof(prog->tag) * 2 + 1] = { };

	bin2hex(prog_tag, prog->tag, sizeof(prog->tag));
	seq_printf(m,
		   "link_type:\t%s\n"
		   "link_id:\t%u\n"
		   "prog_tag:\t%s\n"
		   "prog_id:\t%u\n",
		   bpf_link_type_strs[link->type],
		   link->id,
		   prog_tag,
		   prog->aux->id);
	if (link->ops->show_fdinfo)
		link->ops->show_fdinfo(link, m);
}