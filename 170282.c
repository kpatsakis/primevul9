static int bpf_obj_get_info_by_fd(const union bpf_attr *attr,
				  union bpf_attr __user *uattr)
{
	int ufd = attr->info.bpf_fd;
	struct fd f;
	int err;

	if (CHECK_ATTR(BPF_OBJ_GET_INFO_BY_FD))
		return -EINVAL;

	f = fdget(ufd);
	if (!f.file)
		return -EBADFD;

	if (f.file->f_op == &bpf_prog_fops)
		err = bpf_prog_get_info_by_fd(f.file, f.file->private_data, attr,
					      uattr);
	else if (f.file->f_op == &bpf_map_fops)
		err = bpf_map_get_info_by_fd(f.file, f.file->private_data, attr,
					     uattr);
	else if (f.file->f_op == &btf_fops)
		err = bpf_btf_get_info_by_fd(f.file, f.file->private_data, attr, uattr);
	else if (f.file->f_op == &bpf_link_fops)
		err = bpf_link_get_info_by_fd(f.file, f.file->private_data,
					      attr, uattr);
	else
		err = -EINVAL;

	fdput(f);
	return err;
}