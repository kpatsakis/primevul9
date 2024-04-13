static inline int emulate_grp3(struct x86_emulate_ctxt *ctxt,
			       struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc = 0;

	switch (c->modrm_reg) {
	case 0 ... 1:	/* test */
		emulate_2op_SrcV("test", c->src, c->dst, ctxt->eflags);
		break;
	case 2:	/* not */
		c->dst.val = ~c->dst.val;
		break;
	case 3:	/* neg */
		emulate_1op("neg", c->dst, ctxt->eflags);
		break;
	default:
		DPRINTF("Cannot emulate %02x\n", c->b);
		rc = X86EMUL_UNHANDLEABLE;
		break;
	}
	return rc;
}