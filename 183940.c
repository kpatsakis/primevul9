static int decode_abs(struct x86_emulate_ctxt *ctxt,
		      struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc = 0;

	switch (c->ad_bytes) {
	case 2:
		c->modrm_ea = insn_fetch(u16, 2, c->eip);
		break;
	case 4:
		c->modrm_ea = insn_fetch(u32, 4, c->eip);
		break;
	case 8:
		c->modrm_ea = insn_fetch(u64, 8, c->eip);
		break;
	}
done:
	return rc;
}