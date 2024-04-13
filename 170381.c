static void bpf_tracing_link_dealloc(struct bpf_link *link)
{
	struct bpf_tracing_link *tr_link =
		container_of(link, struct bpf_tracing_link, link);

	kfree(tr_link);
}