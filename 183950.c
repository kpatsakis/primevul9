static void emulate_push_sreg(struct x86_emulate_ctxt *ctxt, int seg)
{
	struct decode_cache *c = &ctxt->decode;
	struct kvm_segment segment;

	kvm_x86_ops->get_segment(ctxt->vcpu, &segment, seg);

	c->src.val = segment.selector;
	emulate_push(ctxt);
}