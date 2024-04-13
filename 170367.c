static void bpf_audit_prog(const struct bpf_prog *prog, unsigned int op)
{
	struct audit_context *ctx = NULL;
	struct audit_buffer *ab;

	if (WARN_ON_ONCE(op >= BPF_AUDIT_MAX))
		return;
	if (audit_enabled == AUDIT_OFF)
		return;
	if (op == BPF_AUDIT_LOAD)
		ctx = audit_context();
	ab = audit_log_start(ctx, GFP_ATOMIC, AUDIT_BPF);
	if (unlikely(!ab))
		return;
	audit_log_format(ab, "prog-id=%u op=%s",
			 prog->aux->id, bpf_audit_str[op]);
	audit_log_end(ab);
}