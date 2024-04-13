static inline void emulate_push(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;

	c->dst.type  = OP_MEM;
	c->dst.bytes = c->op_bytes;
	c->dst.val = c->src.val;
	register_address_increment(c, &c->regs[VCPU_REGS_RSP], -c->op_bytes);
	c->dst.ptr = (void *) register_address(c, ss_base(ctxt),
					       c->regs[VCPU_REGS_RSP]);
}