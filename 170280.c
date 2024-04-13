struct bpf_prog *bpf_prog_get(u32 ufd)
{
	return __bpf_prog_get(ufd, NULL, false);
}