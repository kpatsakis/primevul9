void bpf_prog_inc(struct bpf_prog *prog)
{
	atomic64_inc(&prog->aux->refcnt);
}