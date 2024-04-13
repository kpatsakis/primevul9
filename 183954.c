static inline void emulate_grp2(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;
	switch (c->modrm_reg) {
	case 0:	/* rol */
		emulate_2op_SrcB("rol", c->src, c->dst, ctxt->eflags);
		break;
	case 1:	/* ror */
		emulate_2op_SrcB("ror", c->src, c->dst, ctxt->eflags);
		break;
	case 2:	/* rcl */
		emulate_2op_SrcB("rcl", c->src, c->dst, ctxt->eflags);
		break;
	case 3:	/* rcr */
		emulate_2op_SrcB("rcr", c->src, c->dst, ctxt->eflags);
		break;
	case 4:	/* sal/shl */
	case 6:	/* sal/shl */
		emulate_2op_SrcB("sal", c->src, c->dst, ctxt->eflags);
		break;
	case 5:	/* shr */
		emulate_2op_SrcB("shr", c->src, c->dst, ctxt->eflags);
		break;
	case 7:	/* sar */
		emulate_2op_SrcB("sar", c->src, c->dst, ctxt->eflags);
		break;
	}
}