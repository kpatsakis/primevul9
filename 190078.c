static void init_kvm_softmmu(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *context = &vcpu->arch.root_mmu;
	struct kvm_mmu_role_regs regs = vcpu_to_role_regs(vcpu);

	kvm_init_shadow_mmu(vcpu, &regs);

	context->get_guest_pgd     = get_cr3;
	context->get_pdptr         = kvm_pdptr_read;
	context->inject_page_fault = kvm_inject_page_fault;
}