static int emulate_popa(struct x86_emulate_ctxt *ctxt,
			struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc = 0;
	int reg = VCPU_REGS_RDI;

	while (reg >= VCPU_REGS_RAX) {
		if (reg == VCPU_REGS_RSP) {
			register_address_increment(c, &c->regs[VCPU_REGS_RSP],
							c->op_bytes);
			--reg;
		}

		rc = emulate_pop(ctxt, ops, &c->regs[reg], c->op_bytes);
		if (rc != 0)
			break;
		--reg;
	}
	return rc;
}