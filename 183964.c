static unsigned long es_base(struct x86_emulate_ctxt *ctxt)
{
	return seg_base(ctxt, VCPU_SREG_ES);
}