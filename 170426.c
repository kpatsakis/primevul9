static int link_detach(union bpf_attr *attr)
{
	struct bpf_link *link;
	int ret;

	if (CHECK_ATTR(BPF_LINK_DETACH))
		return -EINVAL;

	link = bpf_link_get_from_fd(attr->link_detach.link_fd);
	if (IS_ERR(link))
		return PTR_ERR(link);

	if (link->ops->detach)
		ret = link->ops->detach(link);
	else
		ret = -EOPNOTSUPP;

	bpf_link_put(link);
	return ret;
}