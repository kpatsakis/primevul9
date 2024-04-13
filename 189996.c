static void drop_large_spte(struct kvm_vcpu *vcpu, u64 *sptep)
{
	if (__drop_large_spte(vcpu->kvm, sptep)) {
		struct kvm_mmu_page *sp = sptep_to_sp(sptep);

		kvm_flush_remote_tlbs_with_address(vcpu->kvm, sp->gfn,
			KVM_PAGES_PER_HPAGE(sp->role.level));
	}
}