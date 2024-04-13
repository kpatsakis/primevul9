static void bpf_prog_load_fixup_attach_type(union bpf_attr *attr)
{
	switch (attr->prog_type) {
	case BPF_PROG_TYPE_CGROUP_SOCK:
		/* Unfortunately BPF_ATTACH_TYPE_UNSPEC enumeration doesn't
		 * exist so checking for non-zero is the way to go here.
		 */
		if (!attr->expected_attach_type)
			attr->expected_attach_type =
				BPF_CGROUP_INET_SOCK_CREATE;
		break;
	}
}