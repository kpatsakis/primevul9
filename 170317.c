int bpf_link_prime(struct bpf_link *link, struct bpf_link_primer *primer)
{
	struct file *file;
	int fd, id;

	fd = get_unused_fd_flags(O_CLOEXEC);
	if (fd < 0)
		return fd;


	id = bpf_link_alloc_id(link);
	if (id < 0) {
		put_unused_fd(fd);
		return id;
	}

	file = anon_inode_getfile("bpf_link", &bpf_link_fops, link, O_CLOEXEC);
	if (IS_ERR(file)) {
		bpf_link_free_id(id);
		put_unused_fd(fd);
		return PTR_ERR(file);
	}

	primer->link = link;
	primer->file = file;
	primer->fd = fd;
	primer->id = id;
	return 0;
}