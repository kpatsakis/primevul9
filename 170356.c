static void bpf_link_free(struct bpf_link *link)
{
	bpf_link_free_id(link->id);
	if (link->prog) {
		/* detach BPF program, clean up used resources */
		link->ops->release(link);
		bpf_prog_put(link->prog);
	}
	/* free bpf_link and its containing memory */
	link->ops->dealloc(link);
}