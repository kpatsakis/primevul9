void nested_load_control_from_vmcb12(struct vcpu_svm *svm,
				     struct vmcb_control_area *control)
{
	copy_vmcb_control_area(&svm->nested.ctl, control);

	/* Copy it here because nested_svm_check_controls will check it.  */
	svm->nested.ctl.asid           = control->asid;
	svm->nested.ctl.msrpm_base_pa &= ~0x0fffULL;
	svm->nested.ctl.iopm_base_pa  &= ~0x0fffULL;
}