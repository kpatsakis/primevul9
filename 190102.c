static gpa_t nonpaging_gva_to_gpa(struct kvm_vcpu *vcpu, struct kvm_mmu *mmu,
				  gpa_t vaddr, u64 access,
				  struct x86_exception *exception)
{
	if (exception)
		exception->error_code = 0;
	return kvm_translate_gpa(vcpu, mmu, vaddr, access, exception);
}