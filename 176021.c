static int resolve_userfault_fork(struct userfaultfd_ctx *ctx,
				  struct userfaultfd_ctx *new,
				  struct uffd_msg *msg)
{
	int fd;
	struct file *file;
	unsigned int flags = new->flags & UFFD_SHARED_FCNTL_FLAGS;

	fd = get_unused_fd_flags(flags);
	if (fd < 0)
		return fd;

	file = anon_inode_getfile("[userfaultfd]", &userfaultfd_fops, new,
				  O_RDWR | flags);
	if (IS_ERR(file)) {
		put_unused_fd(fd);
		return PTR_ERR(file);
	}

	fd_install(fd, file);
	msg->arg.reserved.reserved1 = 0;
	msg->arg.fork.ufd = fd;

	return 0;
}