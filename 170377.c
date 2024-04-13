static int bpf_prog_detach(const union bpf_attr *attr)
{
	enum bpf_prog_type ptype;

	if (CHECK_ATTR(BPF_PROG_DETACH))
		return -EINVAL;

	ptype = attach_type_to_prog_type(attr->attach_type);

	switch (ptype) {
	case BPF_PROG_TYPE_SK_MSG:
	case BPF_PROG_TYPE_SK_SKB:
		return sock_map_prog_detach(attr, ptype);
	case BPF_PROG_TYPE_LIRC_MODE2:
		return lirc_prog_detach(attr);
	case BPF_PROG_TYPE_FLOW_DISSECTOR:
		return netns_bpf_prog_detach(attr, ptype);
	case BPF_PROG_TYPE_CGROUP_DEVICE:
	case BPF_PROG_TYPE_CGROUP_SKB:
	case BPF_PROG_TYPE_CGROUP_SOCK:
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
	case BPF_PROG_TYPE_CGROUP_SYSCTL:
	case BPF_PROG_TYPE_SOCK_OPS:
		return cgroup_bpf_prog_detach(attr, ptype);
	default:
		return -EINVAL;
	}
}