void kvm_mmu_invlpg(struct kvm_vcpu *vcpu, gva_t gva)
{
	kvm_mmu_invalidate_gva(vcpu, vcpu->arch.walk_mmu, gva, INVALID_PAGE);
	++vcpu->stat.invlpg;
}