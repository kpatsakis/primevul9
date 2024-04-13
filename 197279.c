u32 bpf_map_fd_sys_lookup_elem(void *ptr)
{
	return ((struct bpf_map *)ptr)->id;
}