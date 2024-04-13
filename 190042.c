static int nonpaging_page_fault(struct kvm_vcpu *vcpu,
				struct kvm_page_fault *fault)
{
	pgprintk("%s: gva %lx error %x\n", __func__, fault->addr, fault->error_code);

	/* This path builds a PAE pagetable, we can map 2mb pages at maximum. */
	fault->max_level = PG_LEVEL_2M;
	return direct_page_fault(vcpu, fault);
}