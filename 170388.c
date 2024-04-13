static struct bpf_prog *__bpf_prog_get(u32 ufd, enum bpf_prog_type *attach_type,
				       bool attach_drv)
{
	struct fd f = fdget(ufd);
	struct bpf_prog *prog;

	prog = ____bpf_prog_get(f);
	if (IS_ERR(prog))
		return prog;
	if (!bpf_prog_get_ok(prog, attach_type, attach_drv)) {
		prog = ERR_PTR(-EINVAL);
		goto out;
	}

	bpf_prog_inc(prog);
out:
	fdput(f);
	return prog;
}