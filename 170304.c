static int bpf_link_get_fd_by_id(const union bpf_attr *attr)
{
	struct bpf_link *link;
	u32 id = attr->link_id;
	int fd;

	if (CHECK_ATTR(BPF_LINK_GET_FD_BY_ID))
		return -EINVAL;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	link = bpf_link_by_id(id);
	if (IS_ERR(link))
		return PTR_ERR(link);

	fd = bpf_link_new_fd(link);
	if (fd < 0)
		bpf_link_put(link);

	return fd;
}