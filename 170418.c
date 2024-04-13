static int bpf_obj_pin(const union bpf_attr *attr)
{
	if (CHECK_ATTR(BPF_OBJ) || attr->file_flags != 0)
		return -EINVAL;

	return bpf_obj_pin_user(attr->bpf_fd, u64_to_user_ptr(attr->pathname));
}