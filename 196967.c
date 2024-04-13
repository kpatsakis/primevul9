static gpa_t FNAME(gva_to_gpa_nested)(struct kvm_vcpu *vcpu, gva_t vaddr,
				      u32 access,
				      struct x86_exception *exception)
{
	struct guest_walker walker;
	gpa_t gpa = UNMAPPED_GVA;
	int r;

	r = FNAME(walk_addr_nested)(&walker, vcpu, vaddr, access);

	if (r) {
		gpa = gfn_to_gpa(walker.gfn);
		gpa |= vaddr & ~PAGE_MASK;
	} else if (exception)
		*exception = walker.fault;

	return gpa;
}