int bpf_link_new_fd(struct bpf_link *link)
{
	return anon_inode_getfd("bpf-link", &bpf_link_fops, link, O_CLOEXEC);
}