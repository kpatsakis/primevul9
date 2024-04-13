static int set_info_rec_size(struct bpf_prog_info *info)
{
	/*
	 * Ensure info.*_rec_size is the same as kernel expected size
	 *
	 * or
	 *
	 * Only allow zero *_rec_size if both _rec_size and _cnt are
	 * zero.  In this case, the kernel will set the expected
	 * _rec_size back to the info.
	 */

	if ((info->nr_func_info || info->func_info_rec_size) &&
	    info->func_info_rec_size != sizeof(struct bpf_func_info))
		return -EINVAL;

	if ((info->nr_line_info || info->line_info_rec_size) &&
	    info->line_info_rec_size != sizeof(struct bpf_line_info))
		return -EINVAL;

	if ((info->nr_jited_line_info || info->jited_line_info_rec_size) &&
	    info->jited_line_info_rec_size != sizeof(__u64))
		return -EINVAL;

	info->func_info_rec_size = sizeof(struct bpf_func_info);
	info->line_info_rec_size = sizeof(struct bpf_line_info);
	info->jited_line_info_rec_size = sizeof(__u64);

	return 0;
}