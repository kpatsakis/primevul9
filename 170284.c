void bpf_link_init(struct bpf_link *link, enum bpf_link_type type,
		   const struct bpf_link_ops *ops, struct bpf_prog *prog)
{
	atomic64_set(&link->refcnt, 1);
	link->type = type;
	link->id = 0;
	link->ops = ops;
	link->prog = prog;
}