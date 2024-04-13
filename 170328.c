static int bpf_btf_get_info_by_fd(struct file *file,
				  struct btf *btf,
				  const union bpf_attr *attr,
				  union bpf_attr __user *uattr)
{
	struct bpf_btf_info __user *uinfo = u64_to_user_ptr(attr->info.info);
	u32 info_len = attr->info.info_len;
	int err;

	err = bpf_check_uarg_tail_zero(uinfo, sizeof(*uinfo), info_len);
	if (err)
		return err;

	return btf_get_info_by_fd(btf, attr, uattr);
}