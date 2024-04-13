static void __bpf_prog_put(struct bpf_prog *prog, bool do_idr_lock)
{
	if (atomic64_dec_and_test(&prog->aux->refcnt)) {
		perf_event_bpf_event(prog, PERF_BPF_EVENT_PROG_UNLOAD, 0);
		bpf_audit_prog(prog, BPF_AUDIT_UNLOAD);
		/* bpf_prog_free_id() must be called first */
		bpf_prog_free_id(prog, do_idr_lock);
		__bpf_prog_put_noref(prog, true);
	}
}