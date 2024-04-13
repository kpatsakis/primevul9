static int tracing_bpf_link_attach(const union bpf_attr *attr, struct bpf_prog *prog)
{
	if (attr->link_create.attach_type != prog->expected_attach_type)
		return -EINVAL;

	if (prog->expected_attach_type == BPF_TRACE_ITER)
		return bpf_iter_link_attach(attr, prog);
	else if (prog->type == BPF_PROG_TYPE_EXT)
		return bpf_tracing_prog_attach(prog,
					       attr->link_create.target_fd,
					       attr->link_create.target_btf_id);
	return -EINVAL;
}