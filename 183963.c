static inline int emulate_grp1a(struct x86_emulate_ctxt *ctxt,
				struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc;

	rc = emulate_pop(ctxt, ops, &c->dst.val, c->dst.bytes);
	if (rc != 0)
		return rc;
	return 0;
}