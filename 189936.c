static struct kvm_mmu_role_regs vcpu_to_role_regs(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu_role_regs regs = {
		.cr0 = kvm_read_cr0_bits(vcpu, KVM_MMU_CR0_ROLE_BITS),
		.cr4 = kvm_read_cr4_bits(vcpu, KVM_MMU_CR4_ROLE_BITS),
		.efer = vcpu->arch.efer,
	};

	return regs;
}