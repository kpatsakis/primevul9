struct bpf_prog *bpf_prog_inc_not_zero(struct bpf_prog *prog)
{
	int refold;

	refold = atomic64_fetch_add_unless(&prog->aux->refcnt, 1, 0);

	if (!refold)
		return ERR_PTR(-ENOENT);

	return prog;
}