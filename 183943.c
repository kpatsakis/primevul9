static inline int emulate_grp9(struct x86_emulate_ctxt *ctxt,
			       struct x86_emulate_ops *ops,
			       unsigned long memop)
{
	struct decode_cache *c = &ctxt->decode;
	u64 old, new;
	int rc;

	rc = ops->read_emulated(memop, &old, 8, ctxt->vcpu);
	if (rc != 0)
		return rc;

	if (((u32) (old >> 0) != (u32) c->regs[VCPU_REGS_RAX]) ||
	    ((u32) (old >> 32) != (u32) c->regs[VCPU_REGS_RDX])) {

		c->regs[VCPU_REGS_RAX] = (u32) (old >> 0);
		c->regs[VCPU_REGS_RDX] = (u32) (old >> 32);
		ctxt->eflags &= ~EFLG_ZF;

	} else {
		new = ((u64)c->regs[VCPU_REGS_RCX] << 32) |
		       (u32) c->regs[VCPU_REGS_RBX];

		rc = ops->cmpxchg_emulated(memop, &old, &new, 8, ctxt->vcpu);
		if (rc != 0)
			return rc;
		ctxt->eflags |= EFLG_ZF;
	}
	return 0;
}