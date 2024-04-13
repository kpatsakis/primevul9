static bool is_net_admin_prog_type(enum bpf_prog_type prog_type)
{
	switch (prog_type) {
	case BPF_PROG_TYPE_SCHED_CLS:
	case BPF_PROG_TYPE_SCHED_ACT:
	case BPF_PROG_TYPE_XDP:
	case BPF_PROG_TYPE_LWT_IN:
	case BPF_PROG_TYPE_LWT_OUT:
	case BPF_PROG_TYPE_LWT_XMIT:
	case BPF_PROG_TYPE_LWT_SEG6LOCAL:
	case BPF_PROG_TYPE_SK_SKB:
	case BPF_PROG_TYPE_SK_MSG:
	case BPF_PROG_TYPE_LIRC_MODE2:
	case BPF_PROG_TYPE_FLOW_DISSECTOR:
	case BPF_PROG_TYPE_CGROUP_DEVICE:
	case BPF_PROG_TYPE_CGROUP_SOCK:
	case BPF_PROG_TYPE_CGROUP_SOCK_ADDR:
	case BPF_PROG_TYPE_CGROUP_SOCKOPT:
	case BPF_PROG_TYPE_CGROUP_SYSCTL:
	case BPF_PROG_TYPE_SOCK_OPS:
	case BPF_PROG_TYPE_EXT: /* extends any prog */
		return true;
	case BPF_PROG_TYPE_CGROUP_SKB:
		/* always unpriv */
	case BPF_PROG_TYPE_SK_REUSEPORT:
		/* equivalent to SOCKET_FILTER. need CAP_BPF only */
	default:
		return false;
	}
}