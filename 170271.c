void bpf_prog_put(struct bpf_prog *prog)
{
	__bpf_prog_put(prog, true);
}