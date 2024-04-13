static unsigned long seg_override_base(struct x86_emulate_ctxt *ctxt,
				       struct decode_cache *c)
{
	if (!c->has_seg_override)
		return 0;

	return seg_base(ctxt, c->seg_override);
}