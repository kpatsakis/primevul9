static int mmu_check_root(struct kvm_vcpu *vcpu, gfn_t root_gfn)
{
	int ret = 0;

	if (!kvm_vcpu_is_visible_gfn(vcpu, root_gfn)) {
		kvm_make_request(KVM_REQ_TRIPLE_FAULT, vcpu);
		ret = 1;
	}

	return ret;
}