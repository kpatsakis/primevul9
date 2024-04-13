void bpf_map_area_free(void *area)
{
	kvfree(area);
}