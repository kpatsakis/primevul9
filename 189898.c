static bool kvm_sync_page(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp,
			 struct list_head *invalid_list)
{
	int ret = vcpu->arch.mmu->sync_page(vcpu, sp);

	if (ret < 0) {
		kvm_mmu_prepare_zap_page(vcpu->kvm, sp, invalid_list);
		return false;
	}

	return !!ret;
}