static int do_fetch_insn_byte(struct x86_emulate_ctxt *ctxt,
			      struct x86_emulate_ops *ops,
			      unsigned long linear, u8 *dest)
{
	struct fetch_cache *fc = &ctxt->decode.fetch;
	int rc;
	int size;

	if (linear < fc->start || linear >= fc->end) {
		size = min(15UL, PAGE_SIZE - offset_in_page(linear));
		rc = ops->read_std(linear, fc->data, size, ctxt->vcpu);
		if (rc)
			return rc;
		fc->start = linear;
		fc->end = linear + size;
	}
	*dest = fc->data[linear - fc->start];
	return 0;
}