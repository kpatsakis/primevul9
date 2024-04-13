void bpf_jit_free(struct bpf_prog *fp)
{
	unsigned long addr = (unsigned long)fp->bpf_func & PAGE_MASK;
	struct bpf_binary_header *header = (void *)addr;

	if (!fp->jited)
		goto free_filter;

	set_memory_rw(addr, header->pages);
	bpf_jit_binary_free(header);

free_filter:
	bpf_prog_unlock_free(fp);
}