static int emulate_ret_far(struct x86_emulate_ctxt *ctxt,
			   struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc;
	unsigned long cs;

	rc = emulate_pop(ctxt, ops, &c->eip, c->op_bytes);
	if (rc)
		return rc;
	if (c->op_bytes == 4)
		c->eip = (u32)c->eip;
	rc = emulate_pop(ctxt, ops, &cs, c->op_bytes);
	if (rc)
		return rc;
	rc = kvm_load_segment_descriptor(ctxt->vcpu, (u16)cs, 1, VCPU_SREG_CS);
	return rc;
}