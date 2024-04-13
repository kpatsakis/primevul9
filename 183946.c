static int emulate_pop_sreg(struct x86_emulate_ctxt *ctxt,
			     struct x86_emulate_ops *ops, int seg)
{
	struct decode_cache *c = &ctxt->decode;
	unsigned long selector;
	int rc;

	rc = emulate_pop(ctxt, ops, &selector, c->op_bytes);
	if (rc != 0)
		return rc;

	rc = kvm_load_segment_descriptor(ctxt->vcpu, (u16)selector, 1, seg);
	return rc;
}