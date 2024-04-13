static bool nested_svm_check_bitmap_pa(struct kvm_vcpu *vcpu, u64 pa, u32 size)
{
	u64 addr = PAGE_ALIGN(pa);

	return kvm_vcpu_is_legal_gpa(vcpu, addr) &&
	    kvm_vcpu_is_legal_gpa(vcpu, addr + size - 1);
}