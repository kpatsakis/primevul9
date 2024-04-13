static int bpf_link_get_info_by_fd(struct file *file,
				  struct bpf_link *link,
				  const union bpf_attr *attr,
				  union bpf_attr __user *uattr)
{
	struct bpf_link_info __user *uinfo = u64_to_user_ptr(attr->info.info);
	struct bpf_link_info info;
	u32 info_len = attr->info.info_len;
	int err;

	err = bpf_check_uarg_tail_zero(uinfo, sizeof(info), info_len);
	if (err)
		return err;
	info_len = min_t(u32, sizeof(info), info_len);

	memset(&info, 0, sizeof(info));
	if (copy_from_user(&info, uinfo, info_len))
		return -EFAULT;

	info.type = link->type;
	info.id = link->id;
	info.prog_id = link->prog->aux->id;

	if (link->ops->fill_link_info) {
		err = link->ops->fill_link_info(link, &info);
		if (err)
			return err;
	}

	if (copy_to_user(uinfo, &info, info_len) ||
	    put_user(info_len, &uattr->info.info_len))
		return -EFAULT;

	return 0;
}