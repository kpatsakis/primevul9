void kvm_mmu_after_set_cpuid(struct kvm_vcpu *vcpu)
{
	/*
	 * Invalidate all MMU roles to force them to reinitialize as CPUID
	 * information is factored into reserved bit calculations.
	 *
	 * Correctly handling multiple vCPU models with respect to paging and
	 * physical address properties) in a single VM would require tracking
	 * all relevant CPUID information in kvm_mmu_page_role. That is very
	 * undesirable as it would increase the memory requirements for
	 * gfn_track (see struct kvm_mmu_page_role comments).  For now that
	 * problem is swept under the rug; KVM's CPUID API is horrific and
	 * it's all but impossible to solve it without introducing a new API.
	 */
	vcpu->arch.root_mmu.mmu_role.ext.valid = 0;
	vcpu->arch.guest_mmu.mmu_role.ext.valid = 0;
	vcpu->arch.nested_mmu.mmu_role.ext.valid = 0;
	kvm_mmu_reset_context(vcpu);

	/*
	 * Changing guest CPUID after KVM_RUN is forbidden, see the comment in
	 * kvm_arch_vcpu_ioctl().
	 */
	KVM_BUG_ON(vcpu->arch.last_vmentry_cpu != -1, vcpu->kvm);
}