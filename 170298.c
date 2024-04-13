static int bpf_raw_tp_link_fill_link_info(const struct bpf_link *link,
					  struct bpf_link_info *info)
{
	struct bpf_raw_tp_link *raw_tp_link =
		container_of(link, struct bpf_raw_tp_link, link);
	char __user *ubuf = u64_to_user_ptr(info->raw_tracepoint.tp_name);
	const char *tp_name = raw_tp_link->btp->tp->name;
	u32 ulen = info->raw_tracepoint.tp_name_len;
	size_t tp_len = strlen(tp_name);

	if (!ulen ^ !ubuf)
		return -EINVAL;

	info->raw_tracepoint.tp_name_len = tp_len + 1;

	if (!ubuf)
		return 0;

	if (ulen >= tp_len + 1) {
		if (copy_to_user(ubuf, tp_name, tp_len + 1))
			return -EFAULT;
	} else {
		char zero = '\0';

		if (copy_to_user(ubuf, tp_name, ulen - 1))
			return -EFAULT;
		if (put_user(zero, ubuf + ulen - 1))
			return -EFAULT;
		return -ENOSPC;
	}

	return 0;
}