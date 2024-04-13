static int bpf_btf_load(const union bpf_attr *attr)
{
	if (CHECK_ATTR(BPF_BTF_LOAD))
		return -EINVAL;

	if (!bpf_capable())
		return -EPERM;

	return btf_new_fd(attr);
}