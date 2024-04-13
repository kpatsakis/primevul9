static int bpf_prog_attach_check_attach_type(const struct bpf_prog *prog,
					     enum bpf_attach_type attach_type)
{
	switch (prog->type) {
	case BPF_PROG_TYPE_CGROUP_SOCK:
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
	case BPF_PROG_TYPE_SK_LOOKUP:
		return attach_type == prog->expected_attach_type ? 0 : -EINVAL;
	case BPF_PROG_TYPE_CGROUP_SKB:
		if (!capable(CAP_NET_ADMIN))
			/* cg-skb progs can be loaded by unpriv user.
			 * check permissions at attach time.
			 */
			return -EPERM;
		return prog->enforce_expected_attach_type &&
			prog->expected_attach_type != attach_type ?
			-EINVAL : 0;
	default:
		return 0;
	}
}