static void bpf_raw_tp_link_release(struct bpf_link *link)
{
	struct bpf_raw_tp_link *raw_tp =
		container_of(link, struct bpf_raw_tp_link, link);

	bpf_probe_unregister(raw_tp->btp, raw_tp->link.prog);
	bpf_put_raw_tracepoint(raw_tp->btp);
}