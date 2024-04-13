static int bpf_prog_attach(const union bpf_attr *attr)
{
	enum bpf_prog_type ptype;
	struct bpf_prog *prog;
	int ret;

	if (CHECK_ATTR(BPF_PROG_ATTACH))
		return -EINVAL;

	if (attr->attach_flags & ~BPF_F_ATTACH_MASK)
		return -EINVAL;

	ptype = attach_type_to_prog_type(attr->attach_type);
	if (ptype == BPF_PROG_TYPE_UNSPEC)
		return -EINVAL;

	prog = bpf_prog_get_type(attr->attach_bpf_fd, ptype);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	if (bpf_prog_attach_check_attach_type(prog, attr->attach_type)) {
		bpf_prog_put(prog);
		return -EINVAL;
	}

	switch (ptype) {
	case BPF_PROG_TYPE_SK_SKB:
	case BPF_PROG_TYPE_SK_MSG:
		ret = sock_map_get_from_fd(attr, prog);
		break;
	case BPF_PROG_TYPE_LIRC_MODE2:
		ret = lirc_prog_attach(attr, prog);
		break;
	case BPF_PROG_TYPE_FLOW_DISSECTOR:
		ret = netns_bpf_prog_attach(attr, prog);
		break;
	case BPF_PROG_TYPE_CGROUP_DEVICE:
	case BPF_PROG_TYPE_CGROUP_SKB:
	case BPF_PROG_TYPE_CGROUP_SOCK:
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
	case BPF_PROG_TYPE_CGROUP_SYSCTL:
	case BPF_PROG_TYPE_SOCK_OPS:
		ret = cgroup_bpf_prog_attach(attr, ptype, prog);
		break;
	default:
		ret = -EINVAL;
	}

	if (ret)
		bpf_prog_put(prog);
	return ret;
}