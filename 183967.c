static int emulate_pop(struct x86_emulate_ctxt *ctxt,
		       struct x86_emulate_ops *ops,
		       void *dest, int len)
{
	struct decode_cache *c = &ctxt->decode;
	int rc;

	rc = ops->read_emulated(register_address(c, ss_base(ctxt),
						 c->regs[VCPU_REGS_RSP]),
				dest, len, ctxt->vcpu);
	if (rc != 0)
		return rc;

	register_address_increment(c, &c->regs[VCPU_REGS_RSP], len);
	return rc;
}