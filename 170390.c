static int link_create(union bpf_attr *attr)
{
	enum bpf_prog_type ptype;
	struct bpf_prog *prog;
	int ret;

	if (CHECK_ATTR(BPF_LINK_CREATE))
		return -EINVAL;

	prog = bpf_prog_get(attr->link_create.prog_fd);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	ret = bpf_prog_attach_check_attach_type(prog,
						attr->link_create.attach_type);
	if (ret)
		goto out;

	if (prog->type == BPF_PROG_TYPE_EXT) {
		ret = tracing_bpf_link_attach(attr, prog);
		goto out;
	}

	ptype = attach_type_to_prog_type(attr->link_create.attach_type);
	if (ptype == BPF_PROG_TYPE_UNSPEC || ptype != prog->type) {
		ret = -EINVAL;
		goto out;
	}

	switch (ptype) {
	case BPF_PROG_TYPE_CGROUP_SKB:
	case BPF_PROG_TYPE_CGROUP_SOCK:
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
	case BPF_PROG_TYPE_SOCK_OPS:
	case BPF_PROG_TYPE_CGROUP_DEVICE:
	case BPF_PROG_TYPE_CGROUP_SYSCTL:
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
		ret = cgroup_bpf_link_attach(attr, prog);
		break;
	case BPF_PROG_TYPE_TRACING:
		ret = tracing_bpf_link_attach(attr, prog);
		break;
	case BPF_PROG_TYPE_FLOW_DISSECTOR:
	case BPF_PROG_TYPE_SK_LOOKUP:
		ret = netns_bpf_link_create(attr, prog);
		break;
#ifdef CONFIG_NET
	case BPF_PROG_TYPE_XDP:
		ret = bpf_xdp_link_attach(attr, prog);
		break;
#endif
	default:
		ret = -EINVAL;
	}

out:
	if (ret < 0)
		bpf_prog_put(prog);
	return ret;
}