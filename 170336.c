void bpf_prog_add(struct bpf_prog *prog, int i)
{
	atomic64_add(i, &prog->aux->refcnt);
}