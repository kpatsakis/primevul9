static void bpf_raw_tp_link_dealloc(struct bpf_link *link)
{
	struct bpf_raw_tp_link *raw_tp =
		container_of(link, struct bpf_raw_tp_link, link);

	kfree(raw_tp);
}