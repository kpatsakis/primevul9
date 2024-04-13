bool bpf_prog_get_ok(struct bpf_prog *prog,
			    enum bpf_prog_type *attach_type, bool attach_drv)
{
	/* not an attachment, just a refcount inc, always allow */
	if (!attach_type)
		return true;

	if (prog->type != *attach_type)
		return false;
	if (bpf_prog_is_dev_bound(prog->aux) && !attach_drv)
		return false;

	return true;
}