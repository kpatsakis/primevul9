static u64 nested_svm_get_tdp_pdptr(struct kvm_vcpu *vcpu, int index)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	u64 cr3 = svm->nested.ctl.nested_cr3;
	u64 pdpte;
	int ret;

	ret = kvm_vcpu_read_guest_page(vcpu, gpa_to_gfn(cr3), &pdpte,
				       offset_in_page(cr3) + index * 8, 8);
	if (ret)
		return 0;
	return pdpte;
}