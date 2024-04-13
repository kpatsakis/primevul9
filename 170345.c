bpf_prog_load_check_attach(enum bpf_prog_type prog_type,
			   enum bpf_attach_type expected_attach_type,
			   struct btf *attach_btf, u32 btf_id,
			   struct bpf_prog *dst_prog)
{
	if (btf_id) {
		if (btf_id > BTF_MAX_TYPE)
			return -EINVAL;

		if (!attach_btf && !dst_prog)
			return -EINVAL;

		switch (prog_type) {
		case BPF_PROG_TYPE_TRACING:
		case BPF_PROG_TYPE_LSM:
		case BPF_PROG_TYPE_STRUCT_OPS:
		case BPF_PROG_TYPE_EXT:
			break;
		default:
			return -EINVAL;
		}
	}

	if (attach_btf && (!btf_id || dst_prog))
		return -EINVAL;

	if (dst_prog && prog_type != BPF_PROG_TYPE_TRACING &&
	    prog_type != BPF_PROG_TYPE_EXT)
		return -EINVAL;

	switch (prog_type) {
	case BPF_PROG_TYPE_CGROUP_SOCK:
		switch (expected_attach_type) {
		case BPF_CGROUP_INET_SOCK_CREATE:
		case BPF_CGROUP_INET_SOCK_RELEASE:
		case BPF_CGROUP_INET4_POST_BIND:
		case BPF_CGROUP_INET6_POST_BIND:
			return 0;
		default:
			return -EINVAL;
		}
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
		switch (expected_attach_type) {
		case BPF_CGROUP_INET4_BIND:
		case BPF_CGROUP_INET6_BIND:
		case BPF_CGROUP_INET4_CONNECT:
		case BPF_CGROUP_INET6_CONNECT:
		case BPF_CGROUP_INET4_GETPEERNAME:
		case BPF_CGROUP_INET6_GETPEERNAME:
		case BPF_CGROUP_INET4_GETSOCKNAME:
		case BPF_CGROUP_INET6_GETSOCKNAME:
		case BPF_CGROUP_UDP4_SENDMSG:
		case BPF_CGROUP_UDP6_SENDMSG:
		case BPF_CGROUP_UDP4_RECVMSG:
		case BPF_CGROUP_UDP6_RECVMSG:
			return 0;
		default:
			return -EINVAL;
		}
	case BPF_PROG_TYPE_CGROUP_SKB:
		switch (expected_attach_type) {
		case BPF_CGROUP_INET_INGRESS:
		case BPF_CGROUP_INET_EGRESS:
			return 0;
		default:
			return -EINVAL;
		}
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
		switch (expected_attach_type) {
		case BPF_CGROUP_SETSOCKOPT:
		case BPF_CGROUP_GETSOCKOPT:
			return 0;
		default:
			return -EINVAL;
		}
	case BPF_PROG_TYPE_SK_LOOKUP:
		if (expected_attach_type == BPF_SK_LOOKUP)
			return 0;
		return -EINVAL;
	case BPF_PROG_TYPE_EXT:
		if (expected_attach_type)
			return -EINVAL;
		fallthrough;
	default:
		return 0;
	}
}