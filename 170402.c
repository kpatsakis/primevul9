static void bpf_tracing_link_release(struct bpf_link *link)
{
	struct bpf_tracing_link *tr_link =
		container_of(link, struct bpf_tracing_link, link);

	WARN_ON_ONCE(bpf_trampoline_unlink_prog(link->prog,
						tr_link->trampoline));

	bpf_trampoline_put(tr_link->trampoline);

	/* tgt_prog is NULL if target is a kernel function */
	if (tr_link->tgt_prog)
		bpf_prog_put(tr_link->tgt_prog);
}