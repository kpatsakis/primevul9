static void bpf_link_put_deferred(struct work_struct *work)
{
	struct bpf_link *link = container_of(work, struct bpf_link, work);

	bpf_link_free(link);
}