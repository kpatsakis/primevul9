static void emulate_pusha(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;
	unsigned long old_esp = c->regs[VCPU_REGS_RSP];
	int reg = VCPU_REGS_RAX;

	while (reg <= VCPU_REGS_RDI) {
		(reg == VCPU_REGS_RSP) ?
		(c->src.val = old_esp) : (c->src.val = c->regs[reg]);

		emulate_push(ctxt);
		++reg;
	}
}