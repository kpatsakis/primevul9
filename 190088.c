int kvm_tdp_page_fault(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	while (fault->max_level > PG_LEVEL_4K) {
		int page_num = KVM_PAGES_PER_HPAGE(fault->max_level);
		gfn_t base = (fault->addr >> PAGE_SHIFT) & ~(page_num - 1);

		if (kvm_mtrr_check_gfn_range_consistency(vcpu, base, page_num))
			break;

		--fault->max_level;
	}

	return direct_page_fault(vcpu, fault);
}