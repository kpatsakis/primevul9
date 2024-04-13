static int nested_svm_intercept_ioio(struct vcpu_svm *svm)
{
	unsigned port, size, iopm_len;
	u16 val, mask;
	u8 start_bit;
	u64 gpa;

	if (!(vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_IOIO_PROT)))
		return NESTED_EXIT_HOST;

	port = svm->vmcb->control.exit_info_1 >> 16;
	size = (svm->vmcb->control.exit_info_1 & SVM_IOIO_SIZE_MASK) >>
		SVM_IOIO_SIZE_SHIFT;
	gpa  = svm->nested.ctl.iopm_base_pa + (port / 8);
	start_bit = port % 8;
	iopm_len = (start_bit + size > 8) ? 2 : 1;
	mask = (0xf >> (4 - size)) << start_bit;
	val = 0;

	if (kvm_vcpu_read_guest(&svm->vcpu, gpa, &val, iopm_len))
		return NESTED_EXIT_DONE;

	return (val & mask) ? NESTED_EXIT_DONE : NESTED_EXIT_HOST;
}