struct bpf_link *bpf_link_get_from_fd(u32 ufd)
{
	struct fd f = fdget(ufd);
	struct bpf_link *link;

	if (!f.file)
		return ERR_PTR(-EBADF);
	if (f.file->f_op != &bpf_link_fops) {
		fdput(f);
		return ERR_PTR(-EINVAL);
	}

	link = f.file->private_data;
	bpf_link_inc(link);
	fdput(f);

	return link;
}