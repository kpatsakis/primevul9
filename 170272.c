static int bpf_tracing_link_fill_link_info(const struct bpf_link *link,
					   struct bpf_link_info *info)
{
	struct bpf_tracing_link *tr_link =
		container_of(link, struct bpf_tracing_link, link);

	info->tracing.attach_type = tr_link->attach_type;

	return 0;
}