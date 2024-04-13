struct bpf_prog *bpf_prog_get_type_dev(u32 ufd, enum bpf_prog_type type,
				       bool attach_drv)
{
	return __bpf_prog_get(ufd, &type, attach_drv);
}