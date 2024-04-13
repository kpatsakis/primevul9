static bool kvm_arch_setup_async_pf(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa,
				    gfn_t gfn)
{
	struct kvm_arch_async_pf arch;

	arch.token = alloc_apf_token(vcpu);
	arch.gfn = gfn;
	arch.direct_map = vcpu->arch.mmu->direct_map;
	arch.cr3 = vcpu->arch.mmu->get_guest_pgd(vcpu);

	return kvm_setup_async_pf(vcpu, cr2_or_gpa,
				  kvm_vcpu_gfn_to_hva(vcpu, gfn), &arch);
}