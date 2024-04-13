setup_syscalls_segments(struct x86_emulate_ctxt *ctxt,
	struct kvm_segment *cs, struct kvm_segment *ss)
{
	memset(cs, 0, sizeof(struct kvm_segment));
	kvm_x86_ops->get_segment(ctxt->vcpu, cs, VCPU_SREG_CS);
	memset(ss, 0, sizeof(struct kvm_segment));

	cs->l = 0;		/* will be adjusted later */
	cs->base = 0;		/* flat segment */
	cs->g = 1;		/* 4kb granularity */
	cs->limit = 0xffffffff;	/* 4GB limit */
	cs->type = 0x0b;	/* Read, Execute, Accessed */
	cs->s = 1;
	cs->dpl = 0;		/* will be adjusted later */
	cs->present = 1;
	cs->db = 1;

	ss->unusable = 0;
	ss->base = 0;		/* flat segment */
	ss->limit = 0xffffffff;	/* 4GB limit */
	ss->g = 1;		/* 4kb granularity */
	ss->s = 1;
	ss->type = 0x03;	/* Read/Write, Accessed */
	ss->db = 1;		/* 32bit stack segment */
	ss->dpl = 0;
	ss->present = 1;
}