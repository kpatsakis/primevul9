static bool nested_vmcb_check_controls(struct kvm_vcpu *vcpu,
				       struct vmcb_control_area *control)
{
	if (CC(!vmcb_is_intercept(control, INTERCEPT_VMRUN)))
		return false;

	if (CC(control->asid == 0))
		return false;

	if (CC((control->nested_ctl & SVM_NESTED_CTL_NP_ENABLE) && !npt_enabled))
		return false;

	if (CC(!nested_svm_check_bitmap_pa(vcpu, control->msrpm_base_pa,
					   MSRPM_SIZE)))
		return false;
	if (CC(!nested_svm_check_bitmap_pa(vcpu, control->iopm_base_pa,
					   IOPM_SIZE)))
		return false;

	return true;
}